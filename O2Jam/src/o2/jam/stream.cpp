#include <o2/jam/stream.h>
#include "internal.h"

#include <windows.h>
#include <bass/bass.h>
#include <bass/bass_fx.h>

// Every stream is a BASS_FX tempo stream (stream->handle) reading from a
// decoding file stream (its source). The tempo stream does the rate changes;
// it frees the source when it's freed (BASS_FX_FREESOURCE).

namespace o2 {
namespace jam {

    static const float MIN_RATE = 0.5f;
    static const float MAX_RATE = 2.0f;

    static float clamp(
        float value,
        float min,
        float max
    ) {
        if (value < min) {
            return min;
        }
        if (value > max) {
            return max;
        }
        return value;
    }

    static bool checkLoaded(const Stream* stream) {
        if (!stream || stream->handle == 0) {
            internal::setError("The stream isn't loaded");
            return false;
        }
        return true;
    }

    // Stops any pending fadeOutAndStop from stopping the stream
    static void cancelStopSync(const Stream* stream) {
        uint32_t sync = internal::getStopSync(stream->handle);
        if (sync) {
            BASS_ChannelRemoveSync(stream->handle, sync);
            internal::setStopSync(stream->handle, 0);
        }
    }

    // Stops and rewinds. Shared by stop and the end of fadeOutAndStop.
    static void stopAndRewind(DWORD handle) {
        BASS_ChannelStop(handle);
        BASS_ChannelSetPosition(handle, 0, BASS_POS_BYTE);
    }

    // Runs on a BASS thread when fadeOutAndStop's slide ends
    static void CALLBACK onFadeOutEnd(
        HSYNC sync,
        DWORD channel,
        DWORD data,
        void* user
    ) {
        stopAndRewind(channel);
    }

    bool load(
        Stream* stream,
        const char* path
    ) {
        unload(stream);
        *stream = {};

        if (!internal::isStarted()) {
            internal::setError("The library isn't started; call init first");
            return false;
        }

        wchar_t widePath[internal::MAX_PATH_CHARS];
        if (!internal::utf8ToWide(path, widePath, internal::MAX_PATH_CHARS)) {
            return false;
        }

        // PRESCAN reads the whole file once so seeking is exact and the
        // length is right, even for MP3s
        HSTREAM source = BASS_StreamCreateFile(
            FALSE,
            widePath,
            0,
            0,
            BASS_STREAM_DECODE | BASS_STREAM_PRESCAN | BASS_SAMPLE_FLOAT | BASS_UNICODE
        );
        if (!source) {
            internal::setBassError("Can't load the stream");
            return false;
        }

        BASS_CHANNELINFO info;
        QWORD lengthBytes = BASS_ChannelGetLength(source, BASS_POS_BYTE);
        if (!BASS_ChannelGetInfo(source, &info) || lengthBytes == (QWORD)-1) {
            internal::setBassError("Can't read the stream's format");
            BASS_StreamFree(source);
            return false;
        }
        double length = BASS_ChannelBytes2Seconds(source, lengthBytes);

        HSTREAM tempo = BASS_FX_TempoCreate(source, BASS_FX_FREESOURCE);
        if (!tempo) {
            internal::setBassError("Can't create the tempo stream");
            BASS_StreamFree(source);
            return false;
        }

        if (!internal::addStream(tempo)) {
            BASS_StreamFree(tempo);
            return false;
        }

        stream->handle = tempo;
        stream->baseFrequency = (float)info.freq;
        stream->length = length;
        stream->rate = 1.0f;
        stream->rateMode = RateMode::KeepPitch;
        stream->looping = false;
        return true;
    }

    void unload(Stream* stream) {
        if (!stream || stream->handle == 0) {
            return;
        }
        internal::removeStream(stream->handle);
        BASS_StreamFree(stream->handle); // also frees the source
        stream->handle = 0;
    }

    bool isLoaded(const Stream* stream) {
        return stream && stream->handle != 0;
    }

    bool play(Stream* stream) {
        if (!checkLoaded(stream)) {
            return false;
        }

        DWORD state = BASS_ChannelIsActive(stream->handle);
        if (state == BASS_ACTIVE_PLAYING || state == BASS_ACTIVE_STALLED) {
            return true;
        }

        // A stream that played to its end is stopped with its source used up.
        // (Paused near the end, the source can be used up too, because the
        // tempo stream reads ahead, so only rewind when stopped.)
        if (state == BASS_ACTIVE_STOPPED) {
            DWORD source = BASS_FX_TempoGetSource(stream->handle);
            if (BASS_ChannelIsActive(source) == BASS_ACTIVE_STOPPED) {
                BASS_ChannelSetPosition(stream->handle, 0, BASS_POS_BYTE);
            }
        }

        if (!BASS_ChannelPlay(stream->handle, FALSE)) {
            internal::setBassError("Can't play the stream");
            return false;
        }
        return true;
    }

    bool pause(Stream* stream) {
        if (!checkLoaded(stream)) {
            return false;
        }
        if (BASS_ChannelIsActive(stream->handle) == BASS_ACTIVE_PLAYING) {
            BASS_ChannelPause(stream->handle);
        }
        return true;
    }

    bool stop(Stream* stream) {
        if (!checkLoaded(stream)) {
            return false;
        }

        cancelStopSync(stream);

        // Setting the volume to where a fade has got to ends the fade
        if (BASS_ChannelIsSliding(stream->handle, BASS_ATTRIB_VOL)) {
            float volume = 0.0f;
            BASS_ChannelGetAttribute(stream->handle, BASS_ATTRIB_VOL, &volume);
            BASS_ChannelSetAttribute(stream->handle, BASS_ATTRIB_VOL, volume);
        }

        stopAndRewind(stream->handle);
        return true;
    }

    bool isPlaying(const Stream* stream) {
        if (!stream || stream->handle == 0) {
            return false;
        }
        DWORD state = BASS_ChannelIsActive(stream->handle);
        return state == BASS_ACTIVE_PLAYING || state == BASS_ACTIVE_STALLED;
    }

    bool seek(
        Stream* stream,
        double seconds
    ) {
        if (!checkLoaded(stream)) {
            return false;
        }
        if (seconds < 0.0 || seconds > stream->length) {
            internal::setError("The position is outside the stream");
            return false;
        }

        QWORD bytes = BASS_ChannelSeconds2Bytes(stream->handle, seconds);
        if (!BASS_ChannelSetPosition(stream->handle, bytes, BASS_POS_BYTE)) {
            internal::setBassError("Can't seek the stream");
            return false;
        }
        return true;
    }

    double getPosition(const Stream* stream) {
        if (!stream || stream->handle == 0) {
            return -1.0;
        }
        QWORD bytes = BASS_ChannelGetPosition(stream->handle, BASS_POS_BYTE);
        if (bytes == (QWORD)-1) {
            return -1.0;
        }
        return BASS_ChannelBytes2Seconds(stream->handle, bytes);
    }

    bool setLooping(
        Stream* stream,
        bool looping
    ) {
        if (!checkLoaded(stream)) {
            return false;
        }
        DWORD flags = looping ? BASS_SAMPLE_LOOP : 0;
        if (BASS_ChannelFlags(stream->handle, flags, BASS_SAMPLE_LOOP) == (DWORD)-1) {
            internal::setBassError("Can't set looping");
            return false;
        }
        stream->looping = looping;
        return true;
    }

    bool setRate(
        Stream* stream,
        float rate,
        RateMode mode
    ) {
        if (!checkLoaded(stream)) {
            return false;
        }
        if (!(rate >= MIN_RATE && rate <= MAX_RATE)) {
            internal::setError("The rate must be from 0.5 to 2");
            return false;
        }

        // TEMPO changes speed only, in percent (0 = normal). TEMPO_FREQ
        // changes the playback sample rate, which moves pitch with speed.
        // Exactly one of them does the work; the other is reset.
        bool ok;
        if (mode == RateMode::KeepPitch) {
            ok = BASS_ChannelSetAttribute(stream->handle, BASS_ATTRIB_TEMPO_FREQ, stream->baseFrequency)
                && BASS_ChannelSetAttribute(stream->handle, BASS_ATTRIB_TEMPO, (rate - 1.0f) * 100.0f);
        } else {
            ok = BASS_ChannelSetAttribute(stream->handle, BASS_ATTRIB_TEMPO, 0.0f)
                && BASS_ChannelSetAttribute(stream->handle, BASS_ATTRIB_TEMPO_FREQ, stream->baseFrequency * rate);
        }
        if (!ok) {
            internal::setBassError("Can't set the rate");
            return false;
        }

        stream->rate = rate;
        stream->rateMode = mode;
        return true;
    }

    bool setVolume(
        Stream* stream,
        float volume
    ) {
        if (!checkLoaded(stream)) {
            return false;
        }
        cancelStopSync(stream);
        // Setting an attribute also ends a slide of it
        if (!BASS_ChannelSetAttribute(stream->handle, BASS_ATTRIB_VOL, clamp(volume, 0.0f, 1.0f))) {
            internal::setBassError("Can't set the volume");
            return false;
        }
        return true;
    }

    float getVolume(const Stream* stream) {
        float volume = 0.0f;
        if (stream && stream->handle != 0) {
            BASS_ChannelGetAttribute(stream->handle, BASS_ATTRIB_VOL, &volume);
        }
        return volume;
    }

    bool setPan(
        Stream* stream,
        float pan
    ) {
        if (!checkLoaded(stream)) {
            return false;
        }
        if (!BASS_ChannelSetAttribute(stream->handle, BASS_ATTRIB_PAN, clamp(pan, -1.0f, 1.0f))) {
            internal::setBassError("Can't set the pan");
            return false;
        }
        return true;
    }

    float getPan(const Stream* stream) {
        float pan = 0.0f;
        if (stream && stream->handle != 0) {
            BASS_ChannelGetAttribute(stream->handle, BASS_ATTRIB_PAN, &pan);
        }
        return pan;
    }

    bool fadeVolume(
        Stream* stream,
        float volume,
        float seconds
    ) {
        if (!checkLoaded(stream)) {
            return false;
        }
        if (seconds <= 0.0f) {
            return setVolume(stream, volume);
        }

        cancelStopSync(stream);
        // A new slide replaces one in progress, from wherever it has got to
        DWORD milliseconds = (DWORD)(seconds * 1000.0f + 0.5f);
        if (!BASS_ChannelSlideAttribute(stream->handle, BASS_ATTRIB_VOL, clamp(volume, 0.0f, 1.0f), milliseconds)) {
            internal::setBassError("Can't fade the volume");
            return false;
        }
        return true;
    }

    bool fadeOutAndStop(
        Stream* stream,
        float seconds
    ) {
        if (!checkLoaded(stream)) {
            return false;
        }
        if (seconds <= 0.0f || BASS_ChannelIsActive(stream->handle) != BASS_ACTIVE_PLAYING) {
            return stop(stream);
        }

        cancelStopSync(stream);
        DWORD milliseconds = (DWORD)(seconds * 1000.0f + 0.5f);
        if (!BASS_ChannelSlideAttribute(stream->handle, BASS_ATTRIB_VOL, 0.0f, milliseconds)) {
            internal::setBassError("Can't fade the volume");
            return false;
        }

        // Stop when the slide ends. Not done with BASS's negative-volume
        // fade-out, because that stops without rewinding.
        HSYNC sync = BASS_ChannelSetSync(stream->handle, BASS_SYNC_SLIDE | BASS_SYNC_ONETIME, 0, onFadeOutEnd, NULL);
        if (!sync) {
            internal::setBassError("Can't schedule the stop");
            return false;
        }
        internal::setStopSync(stream->handle, sync);
        return true;
    }

}
}
