#include <o2/jam/audio.h>
#include "internal.h"

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <bass/bass.h>
#include <bass/bass_fx.h>

namespace o2 {
namespace jam {

    struct StreamEntry {
        uint32_t handle;
        uint32_t stopSync; // see internal::getStopSync
    };

    static bool started = false;
    static int currentDevice = DEFAULT_DEVICE; // as the user passed it
    static float latency = 0.0f;
    static float streamsVolume = 1.0f;
    static float samplesVolume = 1.0f;

    static StreamEntry streams[O2_JAM_MAX_STREAMS];
    static int streamCount = 0;
    static uint32_t samples[O2_JAM_MAX_SAMPLES];
    static int sampleCount = 0;

    static char errorMessage[256] = "No error";

    static float clamp01(float value) {
        if (value < 0.0f) {
            return 0.0f;
        }
        if (value > 1.0f) {
            return 1.0f;
        }
        return value;
    }

    // BASS reads BASS_CONFIG_UNICODE only before devices are first listed or
    // started, so set it before either
    static void setUpConfig() {
        static bool done = false;
        if (done) {
            return;
        }
        BASS_SetConfig(BASS_CONFIG_UNICODE, TRUE);    // device names in UTF-8
        BASS_SetConfig(BASS_CONFIG_DEV_DEFAULT, TRUE); // the "Default" device (on by default; made explicit)
        done = true;
    }

    // BASS's number for the "Default" entry in the device list. BASS puts it
    // first, and names it itself, so the name isn't translated.
    static bool isDefaultEntry(DWORD bassDevice) {
        BASS_DEVICEINFO info;
        if (!BASS_GetDeviceInfo(bassDevice, &info)) {
            return false;
        }
        return bassDevice == 1 && strcmp(info.name, "Default") == 0;
    }

    // The device BASS_Init(-1) would pick: the "Default" entry, or failing
    // that the device Windows reports as default. setDevice needs a real
    // number to compare and move channels to.
    static DWORD defaultBassDevice() {
        if (isDefaultEntry(1)) {
            return 1;
        }
        BASS_DEVICEINFO info;
        for (DWORD d = 1; BASS_GetDeviceInfo(d, &info); d++) {
            if ((info.flags & BASS_DEVICE_DEFAULT) && (info.flags & BASS_DEVICE_ENABLED)) {
                return d;
            }
        }
        return 1;
    }

    static DWORD toBassDevice(int deviceIndex) {
        if (deviceIndex == DEFAULT_DEVICE) {
            return (DWORD)-1;
        }
        return (DWORD)deviceIndex;
    }

    static int toUserDevice(int deviceIndex) {
        if (deviceIndex != DEFAULT_DEVICE && isDefaultEntry((DWORD)deviceIndex)) {
            return DEFAULT_DEVICE;
        }
        return deviceIndex;
    }

    static float measureLatency() {
        BASS_INFO info;
        if (!BASS_GetInfo(&info)) {
            return 0.0f;
        }
        return (float)info.latency / 1000.0f;
    }

    static void applyGlobalVolumes() {
        BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, (DWORD)(streamsVolume * 10000.0f + 0.5f));
        BASS_SetConfig(BASS_CONFIG_GVOL_SAMPLE, (DWORD)(samplesVolume * 10000.0f + 0.5f));
    }

    bool init(int deviceIndex) {
        if (started) {
            internal::setError("The library is already started; call shutdown first");
            return false;
        }
        if (HIWORD(BASS_GetVersion()) != BASSVERSION) {
            internal::setError("bass.dll is a different version than bass.h");
            return false;
        }
        if (HIWORD(BASS_FX_GetVersion()) != BASSVERSION) {
            internal::setError("bass_fx.dll is a different version than bass_fx.h");
            return false;
        }

        setUpConfig();

        if (!BASS_Init(toBassDevice(deviceIndex), 48000, 0, 0, NULL)) {
            internal::setBassError("Can't start the audio device");
            return false;
        }

        started = true;
        currentDevice = toUserDevice(deviceIndex);
        latency = measureLatency();
        applyGlobalVolumes();
        return true;
    }

    void shutdown() {
        if (!started) {
            return;
        }

        // BASS_Free frees the device's channels, but a tempo stream's source
        // is a decoding channel; free each stream explicitly so the source
        // goes with it (BASS_FX_FREESOURCE)
        for (int i = 0; i < streamCount; i++) {
            BASS_StreamFree(streams[i].handle);
        }
        for (int i = 0; i < sampleCount; i++) {
            BASS_SampleFree(samples[i]);
        }
        streamCount = 0;
        sampleCount = 0;

        BASS_Free();
        started = false;
        currentDevice = DEFAULT_DEVICE;
        latency = 0.0f;
    }

    int listDevices(
        Device* devices,
        int capacity
    ) {
        setUpConfig();

        int count = 0;
        BASS_DEVICEINFO info;
        // device 0 is BASS's "no sound" device; real devices start at 1
        for (DWORD d = 1; BASS_GetDeviceInfo(d, &info); d++) {
            if (devices && count < capacity) {
                Device* device = &devices[count];
                device->index = (int)d;
                snprintf(device->name, sizeof(device->name), "%s", info.name ? info.name : "");
                snprintf(device->driver, sizeof(device->driver), "%s", info.driver ? info.driver : "");
                device->isDefault = (info.flags & BASS_DEVICE_DEFAULT) != 0;
                device->isEnabled = (info.flags & BASS_DEVICE_ENABLED) != 0;
            }
            count++;
        }
        return count;
    }

    bool setDevice(int deviceIndex) {
        if (!started) {
            internal::setError("The library isn't started; call init first");
            return false;
        }

        DWORD oldDevice = BASS_GetDevice();
        DWORD newDevice = toBassDevice(deviceIndex);
        if (deviceIndex == DEFAULT_DEVICE) {
            newDevice = defaultBassDevice();
        }
        if (newDevice == oldDevice) {
            return true;
        }

        // BASS_Init makes the new device current for this thread
        if (!BASS_Init(newDevice, 48000, 0, 0, NULL)) {
            internal::setBassError("Can't start the audio device");
            BASS_SetDevice(oldDevice);
            return false;
        }

        // Move everything. A tempo stream reads from a decoding channel that
        // also belongs to the old device, so move that too, or BASS_Free
        // below would free it.
        for (int i = 0; i < streamCount; i++) {
            DWORD source = BASS_FX_TempoGetSource(streams[i].handle);
            if (source) {
                BASS_ChannelSetDevice(source, newDevice);
            }
            BASS_ChannelSetDevice(streams[i].handle, newDevice);
        }
        for (int i = 0; i < sampleCount; i++) {
            BASS_ChannelSetDevice(samples[i], newDevice);
        }

        BASS_SetDevice(oldDevice);
        BASS_Free();
        BASS_SetDevice(newDevice);

        currentDevice = toUserDevice(deviceIndex);
        latency = measureLatency();
        return true;
    }

    int getDevice() {
        return currentDevice;
    }

    float getLatency() {
        return latency;
    }

    void setStreamsVolume(float volume) {
        streamsVolume = clamp01(volume);
        if (started) {
            applyGlobalVolumes();
        }
    }

    void setSamplesVolume(float volume) {
        samplesVolume = clamp01(volume);
        if (started) {
            applyGlobalVolumes();
        }
    }

    float getStreamsVolume() {
        return streamsVolume;
    }

    float getSamplesVolume() {
        return samplesVolume;
    }

    const char* getErrorMessage() {
        return errorMessage;
    }

namespace internal {

    bool isStarted() {
        return started;
    }

    void setError(const char* message) {
        snprintf(errorMessage, sizeof(errorMessage), "%s", message);
    }

    static const char* describeBassError(int code) {
        switch (code) {
        case BASS_ERROR_MEM:         return "out of memory";
        case BASS_ERROR_FILEOPEN:    return "can't open the file";
        case BASS_ERROR_DRIVER:      return "no free or valid driver";
        case BASS_ERROR_HANDLE:      return "invalid handle";
        case BASS_ERROR_FORMAT:      return "unsupported sample format";
        case BASS_ERROR_POSITION:    return "invalid position";
        case BASS_ERROR_INIT:        return "the device isn't started";
        case BASS_ERROR_REINIT:      return "the device needs to be restarted";
        case BASS_ERROR_ALREADY:     return "already started";
        case BASS_ERROR_NOTAUDIO:    return "the file has no audio";
        case BASS_ERROR_NOCHAN:      return "no free channel";
        case BASS_ERROR_ILLTYPE:     return "invalid type";
        case BASS_ERROR_ILLPARAM:    return "invalid parameter";
        case BASS_ERROR_DEVICE:      return "invalid device";
        case BASS_ERROR_NOPLAY:      return "not playing";
        case BASS_ERROR_FREQ:        return "invalid sample rate";
        case BASS_ERROR_EMPTY:       return "the file has no sample data";
        case BASS_ERROR_NOTAVAIL:    return "not available";
        case BASS_ERROR_DECODE:      return "wrong kind of channel";
        case BASS_ERROR_FILEFORM:    return "unsupported file format";
        case BASS_ERROR_SPEAKER:     return "unavailable speaker";
        case BASS_ERROR_VERSION:     return "add-on is for a different BASS version";
        case BASS_ERROR_CODEC:       return "codec not available";
        case BASS_ERROR_ENDED:       return "the channel has ended";
        case BASS_ERROR_BUSY:        return "the device is busy";
        case BASS_ERROR_DENIED:      return "access denied";
        default:                     return "unknown error";
        }
    }

    void setBassError(const char* what) {
        int code = BASS_ErrorGetCode();
        snprintf(errorMessage, sizeof(errorMessage), "%s (%s, BASS error %d)", what, describeBassError(code), code);
    }

    bool utf8ToWide(
        const char* utf8,
        wchar_t* wide,
        int capacity
    ) {
        int written = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8, -1, wide, capacity);
        if (written == 0) {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
                setError("The path is too long");
            } else {
                setError("The path isn't valid UTF-8");
            }
            return false;
        }
        return true;
    }

    bool addStream(uint32_t handle) {
        if (streamCount >= O2_JAM_MAX_STREAMS) {
            char message[96];
            snprintf(message, sizeof(message), "Too many streams loaded (O2_JAM_MAX_STREAMS is %d)", O2_JAM_MAX_STREAMS);
            setError(message);
            return false;
        }
        streams[streamCount].handle = handle;
        streams[streamCount].stopSync = 0;
        streamCount++;
        return true;
    }

    void removeStream(uint32_t handle) {
        for (int i = 0; i < streamCount; i++) {
            if (streams[i].handle == handle) {
                streams[i] = streams[streamCount - 1];
                streamCount--;
                return;
            }
        }
    }

    bool addSample(uint32_t handle) {
        if (sampleCount >= O2_JAM_MAX_SAMPLES) {
            char message[96];
            snprintf(message, sizeof(message), "Too many samples loaded (O2_JAM_MAX_SAMPLES is %d)", O2_JAM_MAX_SAMPLES);
            setError(message);
            return false;
        }
        samples[sampleCount] = handle;
        sampleCount++;
        return true;
    }

    void removeSample(uint32_t handle) {
        for (int i = 0; i < sampleCount; i++) {
            if (samples[i] == handle) {
                samples[i] = samples[sampleCount - 1];
                sampleCount--;
                return;
            }
        }
    }

    uint32_t getStopSync(uint32_t streamHandle) {
        for (int i = 0; i < streamCount; i++) {
            if (streams[i].handle == streamHandle) {
                return streams[i].stopSync;
            }
        }
        return 0;
    }

    void setStopSync(
        uint32_t streamHandle,
        uint32_t sync
    ) {
        for (int i = 0; i < streamCount; i++) {
            if (streams[i].handle == streamHandle) {
                streams[i].stopSync = sync;
                return;
            }
        }
    }

}

}
}
