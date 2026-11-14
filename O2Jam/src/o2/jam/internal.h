#pragma once
#include <stdint.h>
#include <wchar.h>

// Shared between the library's .cpp files. Not part of the public API: only
// files in src/o2/jam/ include this.

namespace o2 {
namespace jam {
namespace internal {

    // Longest path, in UTF-16 code units, that load accepts
    const int MAX_PATH_CHARS = 1024;

    bool isStarted();

    // Sets the message getErrorMessage returns
    void setError(const char* message);

    // Sets the message to "<what> (<BASS's description>, BASS error <code>)"
    // for the error code of the last BASS call that failed
    void setBassError(const char* what);

    // Converts a UTF-8 path to UTF-16 for BASS. Sets the error and returns
    // false if it isn't valid UTF-8 or doesn't fit.
    bool utf8ToWide(
        const char* utf8,
        wchar_t* wide,
        int capacity
    );

    // The handles of everything loaded, so setDevice can move them and
    // shutdown can free them. add returns false (and sets the error) when the
    // list is full.
    bool addStream(uint32_t handle);
    void removeStream(uint32_t handle);
    bool addSample(uint32_t handle);
    void removeSample(uint32_t handle);

    // The sync that stops a stream at the end of fadeOutAndStop, or 0. Kept
    // with the stream's handle so setVolume, fadeVolume and stop can cancel it.
    uint32_t getStopSync(uint32_t streamHandle);
    void setStopSync(
        uint32_t streamHandle,
        uint32_t sync
    );

}
}
}
