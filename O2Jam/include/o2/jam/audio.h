#pragma once

// Setting up audio output: starting and stopping the library, choosing the
// output device, the measured output latency, global volumes, and errors.
// Streams (songs) are in <o2/jam/stream.h>, samples (sound effects) in
// <o2/jam/sample.h>.
//
// Call everything from one thread, the game's main thread.

// How many streams and samples can be loaded at once. The library keeps the
// handle of each one in a fixed array so it can move them all to a new device
// in setDevice. Define these for the whole project to change them.
#ifndef O2_JAM_MAX_STREAMS
#define O2_JAM_MAX_STREAMS 64
#endif

#ifndef O2_JAM_MAX_SAMPLES
#define O2_JAM_MAX_SAMPLES 4096
#endif

namespace o2 {
namespace jam {

    // Pass to init or setDevice for the "Default" device, which follows
    // whatever output device Windows is set to, even when it changes while
    // the game runs.
    const int DEFAULT_DEVICE = -1;

    // An output device, as listed by listDevices.
    struct Device {
        int index;        // pass to init or setDevice
        char name[256];   // the name to show the player, UTF-8
        char driver[256]; // stays the same between runs, unlike index: save this in settings
        bool isDefault;   // Windows' default device: set on the "Default" entry and the device it stands for
        bool isEnabled;   // false when unplugged or disabled; such a device can't be used
    };

    // Starts audio output on a device: DEFAULT_DEVICE, or an index from
    // listDevices. Call before anything else.
    bool init(int deviceIndex);

    // Stops audio output and frees every loaded stream and sample. Stream
    // and Sample structs loaded before must not be used afterwards.
    void shutdown();

    // Fills devices with up to capacity output devices and returns how many
    // there are in total, which can be more than capacity. Can be called
    // before init.
    int listDevices(
        Device* devices,
        int capacity
    );

    // Switches output to another device. Loaded streams and samples move with
    // it, and playing streams keep playing from where they were. Sample
    // playbacks that are going are cut off.
    bool setDevice(int deviceIndex);

    // The index of the device in use, or DEFAULT_DEVICE.
    int getDevice();

    // How long, in seconds, between starting a sound and the device starting
    // to output it, as measured by BASS. Measured in init and setDevice.
    float getLatency();

    // Volume of all streams and of all samples, from 0 (silent) to 1 (the
    // files' own level), multiplied with each one's own volume. Both start
    // at 1, and can be set before init.
    void setStreamsVolume(float volume);
    void setSamplesVolume(float volume);
    float getStreamsVolume();
    float getSamplesVolume();

    // A description of why the last call that failed on this thread failed.
    // Never null.
    const char* getErrorMessage();

}
}
