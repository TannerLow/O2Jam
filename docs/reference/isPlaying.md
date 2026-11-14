# isPlaying

[Index](../index.md)

Tells whether a [Stream](Stream.md) is playing.

```cpp
bool isPlaying(const Stream* stream);
```

`<o2/jam/stream.h>`, namespace `o2::jam`

## Parameters

| | |
|---|---|
| `stream` | The stream to check. |

## Return value

`true` while the stream plays, including while it fades. `false` when it's paused, stopped, has played to its end, or isn't loaded.

## Remarks

A stream that reaches its end stops by itself, unless it's [looping](setLooping.md), so `isPlaying` turning `false` is how to tell a song has finished.

It asks BASS each time, which is cheap but not free; checking once per UI update or game frame is fine.

## Example

```cpp
if (inGameplay && !o2::jam::isPlaying(&song) && !paused) {
	showResults();
}
```

## See also

[play](play.md), [getPosition](getPosition.md)
