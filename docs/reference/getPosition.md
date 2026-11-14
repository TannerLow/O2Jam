# getPosition

[Index](../index.md)

Tells where in the song a [Stream](Stream.md) is.

```cpp
double getPosition(const Stream* stream);
```

`<o2/jam/stream.h>`, namespace `o2::jam`

## Parameters

| | |
|---|---|
| `stream` | The stream to ask about. |

## Return value

The position being heard, in seconds of the song, not affected by [rate](setRate.md): at 2×, it advances 2 seconds every real second. -1 if nothing is loaded.

## Remarks

The position doesn't move smoothly: it jumps forward each time BASS passes a block of audio to the device, every few milliseconds. Read many times a second, it repeats the same value for several frames and then jumps. That's fine for a progress bar or for checking where a song is, but notes drawn from it would stutter. Time gameplay with the game's own clock instead. See [Latency](../concepts/latency.md).

## Example

```cpp
float progress = (float)(o2::jam::getPosition(&song) / song.length);
drawProgressBar(progress);
```

## See also

[seek](seek.md), [Stream](Stream.md), [Rate and pitch](../concepts/rate-and-pitch.md#timing)
