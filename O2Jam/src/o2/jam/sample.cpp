#include <o2/jam/sample.h>
#include "internal.h"

#include <assert.h>
#include <windows.h>
#include <bass/bass.h>

namespace o2 {
namespace jam {

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

    // Asserts on misuse; an unloaded sample is allowed and just fails, since
    // it's what a failed load leaves
    static void assertValid(const Sample* sample) {
        assert(sample && "sample is null");
        assert((sample->handle == 0 || internal::isSampleListed(sample->handle))
            && "sample isn't zero-initialized, was unloaded through a copy, or outlived shutdown");
    }

    static bool checkLoaded(const Sample* sample) {
        assertValid(sample);
        if (!sample || sample->handle == 0) {
            internal::setError("The sample isn't loaded");
            return false;
        }
        return true;
    }

    bool load(
        Sample* sample,
        const char* path,
        int maxPlaybacks
    ) {
        assert(path && "path is null");
        assert(internal::isStarted() && "load called before init");
        assert(maxPlaybacks >= 1 && maxPlaybacks <= 65535 && "maxPlaybacks must be from 1 to 65535");
        unload(sample);
        *sample = {};

        if (!internal::isStarted()) {
            internal::setError("The library isn't started; call init first");
            return false;
        }
        if (maxPlaybacks < 1 || maxPlaybacks > 65535) {
            internal::setError("maxPlaybacks must be from 1 to 65535");
            return false;
        }

        wchar_t widePath[internal::MAX_PATH_CHARS];
        if (!internal::utf8ToWide(path, widePath, internal::MAX_PATH_CHARS)) {
            return false;
        }

        // OVER_POS: when every playback is in use, a new play cuts off the
        // one that has played longest
        HSAMPLE handle = BASS_SampleLoad(
            FALSE,
            widePath,
            0,
            0,
            (DWORD)maxPlaybacks,
            BASS_SAMPLE_OVER_POS | BASS_UNICODE
        );
        if (!handle) {
            internal::setBassError("Can't load the sample");
            return false;
        }

        if (!internal::addSample(handle)) {
            BASS_SampleFree(handle);
            return false;
        }

        sample->handle = handle;
        sample->maxPlaybacks = maxPlaybacks;
        sample->volume = 1.0f;
        sample->pan = 0.0f;
        return true;
    }

    void unload(Sample* sample) {
        assertValid(sample);
        if (!sample || sample->handle == 0) {
            return;
        }
        internal::removeSample(sample->handle);
        BASS_SampleFree(sample->handle); // also stops its playbacks
        sample->handle = 0;
    }

    bool isLoaded(const Sample* sample) {
        assertValid(sample);
        return sample && sample->handle != 0;
    }

    bool play(Sample* sample) {
        if (!checkLoaded(sample)) {
            return false;
        }
        return play(sample, sample->volume, sample->pan);
    }

    bool play(
        Sample* sample,
        float volume,
        float pan
    ) {
        if (!checkLoaded(sample)) {
            return false;
        }

        // A new channel starts paused, so its volume and pan are set before
        // anything is heard
        HCHANNEL channel = BASS_SampleGetChannel(sample->handle, 0);
        if (!channel) {
            internal::setBassError("Can't play the sample");
            return false;
        }
        BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, clamp(volume, 0.0f, 1.0f));
        BASS_ChannelSetAttribute(channel, BASS_ATTRIB_PAN, clamp(pan, -1.0f, 1.0f));

        if (!BASS_ChannelStart(channel)) {
            internal::setBassError("Can't play the sample");
            return false;
        }
        return true;
    }

    bool stop(Sample* sample) {
        if (!checkLoaded(sample)) {
            return false;
        }
        BASS_SampleStop(sample->handle);
        return true;
    }

    bool setVolume(
        Sample* sample,
        float volume
    ) {
        if (!checkLoaded(sample)) {
            return false;
        }
        sample->volume = clamp(volume, 0.0f, 1.0f);
        return true;
    }

    float getVolume(const Sample* sample) {
        assertValid(sample);
        if (!sample || sample->handle == 0) {
            return 0.0f;
        }
        return sample->volume;
    }

    bool setPan(
        Sample* sample,
        float pan
    ) {
        if (!checkLoaded(sample)) {
            return false;
        }
        sample->pan = clamp(pan, -1.0f, 1.0f);
        return true;
    }

    float getPan(const Sample* sample) {
        assertValid(sample);
        if (!sample || sample->handle == 0) {
            return 0.0f;
        }
        return sample->pan;
    }

}
}
