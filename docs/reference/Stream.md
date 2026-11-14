# Stream

[Index](../index.md)

A loaded stream: a song or other long audio, played from its file as it goes. Covers `Stream` and `RateMode`.

```cpp
enum class RateMode {
    KeepPitch,
    ChangePitch
};

struct Stream {
    uint32_t handle;
    float baseFrequency;
    double length;
    float rate;
    RateMode rateMode;
    bool looping;
};
```

`<o2/jam/stream.h>`, namespace `o2::jam`

## Members

| | |
|---|---|
| `handle` | The BASS handle, or 0 when nothing is loaded. |
| `baseFrequency` | The file's sample rate in Hz, such as 44100. |
| `length` | The length in seconds of the song, at rate 1. |
| `rate` | The playback speed, set with [setRate](setRate.md). 1 after [load](load.md). |
| `rateMode` | What the rate does to pitch, set with [setRate](setRate.md). `KeepPitch` after load. |
| `looping` | Whether it loops, set with [setLooping](setLooping.md). `false` after load. |

## RateMode

| | |
|---|---|
| `KeepPitch` | The rate changes only the speed. Like osu!'s double time and half time. |
| `ChangePitch` | Pitch rises and falls with the speed, like a record played faster or slower. Like osu!'s nightcore and daycore. |

## Remarks

A Stream is a plain struct you own and can put anywhere. It doesn't hold the audio, only its handle and settings; BASS holds the audio. Copying a Stream copies the handle, so both copies refer to the same playing song; keep one.

Read the fields freely, but change them only through the functions, which keep BASS in step. Zero-initialize a Stream (`= {}`) before its first [load](load.md), so functions called on it before loading fail safely.

Streams that aren't needed should be [unloaded](unload.md): each keeps its file open and a small buffer.

## Example

```cpp
o2::jam::Stream song = {};
if (o2::jam::load(&song, path)) {
	printf("%.1f seconds at %.0f Hz\n", song.length, song.baseFrequency);
}
```

## See also

[load](load.md), [play](play.md), [setRate](setRate.md), [Sample](Sample.md), [Streams and samples](../concepts/streams-and-samples.md)
