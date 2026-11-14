# play

[Index](../index.md)

Plays a [Stream](Stream.md), or starts a playback of a [Sample](Sample.md).

```cpp
bool play(Stream* stream);

bool play(Sample* sample);

bool play(
    Sample* sample,
    float volume,
    float pan
);
```

`<o2/jam/stream.h>`, `<o2/jam/sample.h>`, namespace `o2::jam`

## Parameters

| | |
|---|---|
| `stream`, `sample` | What to play. |
| `volume` | Samples only: the volume for this playback, from 0 to 1, instead of the sample's own. Clamped. |
| `pan` | Samples only: the pan for this playback, from -1 (left) to 1 (right), instead of the sample's own. Clamped. |

## Return value

`true` if it's playing. `false` if nothing is loaded.

## Remarks

**Streams** play from the current position: the beginning after [load](load.md) or [stop](stop.md), where they were after [pause](pause.md), wherever [seek](seek.md) put them. A stream that played to its end starts again from the beginning. Calling `play` on a stream that's already playing does nothing.

**Samples** start a new playback from the beginning every call, overlapping playbacks that are still going. When `maxPlaybacks` are already going, the one that started longest ago is cut off to make room. `play(sample)` uses the sample's `volume` and `pan`; the other overload uses the ones given, for this playback only.

Either way the sound starts as soon as the device allows, [getLatency](getLatency.md) after the call, and `play` returns without waiting. It doesn't read from disk or allocate, so it's fine to call during gameplay, many times a frame.

## Example

```cpp
o2::jam::play(&song);

// pan each column's hitsound toward its side of the playfield
float pan = (column - (columnCount - 1) * 0.5f) / columnCount;
o2::jam::play(&hit, hit.volume, pan);
```

## See also

[pause](pause.md), [stop](stop.md), [isPlaying](isPlaying.md), [getLatency](getLatency.md)
