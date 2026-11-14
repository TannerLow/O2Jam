# seek

[Index](../index.md)

Moves a [Stream](Stream.md) to a position.

```cpp
bool seek(
    Stream* stream,
    double seconds
);
```

`<o2/jam/stream.h>`, namespace `o2::jam`

## Parameters

| | |
|---|---|
| `stream` | The stream to move. |
| `seconds` | The position, in seconds of the song, from 0 up to its `length`. Not affected by [rate](setRate.md). |

## Return value

`true` if the stream moved. `false` if `seconds` is negative or past the end, or nothing is loaded.

## Remarks

Works whether the stream is playing, paused or stopped, and keeps that state: a playing stream carries on from the new position, after a gap of a few milliseconds while it refills its buffer.

Seeking is exact to the sample, because [load](load.md) scans the file.

## Example

```cpp
// start a song preview at its preview point
o2::jam::seek(&preview, chart.previewTime);
o2::jam::play(&preview);
```

## See also

[getPosition](getPosition.md), [Song previews](../concepts/previews.md)
