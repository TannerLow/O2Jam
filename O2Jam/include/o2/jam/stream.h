#pragma once
#include <stdint.h>

// Streams: long audio, like songs, decoded from the file a little at a time
// as it plays. A stream plays once at a time and can be paused, seeked,
// looped, faded and sped up or slowed down. See <o2/jam/audio.h> for init.

namespace o2 {
namespace jam {

    // What setRate does to pitch.
    enum class RateMode {
        KeepPitch,  // only the speed changes, like osu!'s double time and half time
        ChangePitch // pitch follows speed, like a record played faster: osu!'s nightcore
    };

    // A loaded stream. Fill it with load. Read its fields freely, but change
    // them only through these functions.
    struct Stream {
        uint32_t handle;     // 0 when not loaded
        float baseFrequency; // the file's sample rate, in Hz
        double length;       // in seconds, at rate 1
        float rate;          // set with setRate; starts at 1
        RateMode rateMode;   // set with setRate; starts at KeepPitch
        bool looping;        // set with setLooping; starts false
    };

    // Opens an audio file for streaming. path is UTF-8. A stream already
    // loaded into stream is unloaded first. The stream starts stopped, at
    // the beginning, with volume 1 and pan 0.
    bool load(
        Stream* stream,
        const char* path
    );

    // Stops the stream and closes its file. Does nothing if it isn't loaded.
    void unload(Stream* stream);

    // Starts or resumes playback from the current position. A stream that
    // played to its end starts again from the beginning.
    bool play(Stream* stream);

    // Pauses playback, keeping the position.
    bool pause(Stream* stream);

    // Stops playback and moves back to the beginning.
    bool stop(Stream* stream);

    // Whether the stream is playing. false once it has played to its end,
    // unless it loops.
    bool isPlaying(const Stream* stream);

    // Moves to a position, in seconds of the song (not affected by rate).
    // Works while playing, paused or stopped.
    bool seek(
        Stream* stream,
        double seconds
    );

    // The position being heard, in seconds of the song (not affected by
    // rate), or -1 if the stream isn't loaded.
    double getPosition(const Stream* stream);

    // Whether the stream goes back to the beginning when it ends.
    bool setLooping(
        Stream* stream,
        bool looping
    );

    // Sets the playback speed, from 0.5 to 2 (1 is normal), and whether pitch
    // follows it. Takes effect immediately, also while playing.
    bool setRate(
        Stream* stream,
        float rate,
        RateMode mode
    );

    // Volume from 0 (silent) to 1 (full). Stops a fade in progress.
    bool setVolume(
        Stream* stream,
        float volume
    );

    // Pan from -1 (left) through 0 (center) to 1 (right).
    bool setPan(
        Stream* stream,
        float pan
    );

    // Changes the volume smoothly to volume over seconds, while the stream
    // plays on. Returns immediately.
    bool fadeVolume(
        Stream* stream,
        float volume,
        float seconds
    );

    // Fades the volume to 0 over seconds, then stops the stream as stop does.
    // Returns immediately.
    bool fadeOutAndStop(
        Stream* stream,
        float seconds
    );

}
}
