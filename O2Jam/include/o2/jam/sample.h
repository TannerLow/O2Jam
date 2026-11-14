#pragma once
#include <stdint.h>

// Samples: short sounds, like hitsounds and keysounds, decoded into memory
// when loaded so they start without delay. One sample can play several times
// at once, overlapping. Samples ignore stream rates. See <o2/jam/audio.h>
// for init.

namespace o2 {
namespace jam {

    // A loaded sample. Fill it with load. Read its fields freely, but change
    // them only through these functions.
    struct Sample {
        uint32_t handle;  // 0 when not loaded
        int maxPlaybacks; // how many times it can play at once
        float volume;     // used by play(sample); set with setVolume
        float pan;        // used by play(sample); set with setPan
    };

    // Loads and decodes a whole audio file into memory. path is UTF-8.
    // maxPlaybacks is how many times the sample can play at once; playing it
    // once more cuts off the playback that started longest ago. A sample
    // already loaded into sample is unloaded first. Volume starts at 1 and
    // pan at 0.
    bool load(
        Sample* sample,
        const char* path,
        int maxPlaybacks
    );

    // Stops every playback of the sample and frees its memory. Does nothing
    // if it isn't loaded.
    void unload(Sample* sample);

    // Plays the sample from the start, with its own volume and pan.
    bool play(Sample* sample);

    // Plays the sample from the start, with this volume and pan instead of
    // its own.
    bool play(
        Sample* sample,
        float volume,
        float pan
    );

    // Stops every playback of the sample.
    bool stop(Sample* sample);

    // The volume for later plays, from 0 (silent) to 1 (full). Playbacks
    // already going keep theirs.
    bool setVolume(
        Sample* sample,
        float volume
    );

    // The pan for later plays, from -1 (left) through 0 (center) to 1
    // (right). Playbacks already going keep theirs.
    bool setPan(
        Sample* sample,
        float pan
    );

}
}
