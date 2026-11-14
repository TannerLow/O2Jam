# setPan

[Index](../index.md)

Sets the pan of a [Stream](Stream.md), or of later playbacks of a [Sample](Sample.md).

```cpp
bool setPan(
    Stream* stream,
    float pan
);

bool setPan(
    Sample* sample,
    float pan
);
```

`<o2/jam/stream.h>`, `<o2/jam/sample.h>`, namespace `o2::jam`

## Parameters

| | |
|---|---|
| `stream`, `sample` | What to set the pan of. |
| `pan` | From -1 (all left) through 0 (center) to 1 (all right). Values outside are clamped. |

## Return value

`true` if set. `false` if nothing is loaded.

## Remarks

**Streams** change at once, also while playing.

**Samples** store it in `sample->pan` for later [play](play.md)`(sample)` calls; playbacks already going keep theirs. To pan one playback, use `play(sample, volume, pan)`.

Panning a stereo sound to one side turns the other side's channel down; it doesn't move both channels to one speaker.

## Example

```cpp
o2::jam::setPan(&clap, -0.3f);
```

## See also

[getPan](getPan.md), [setVolume](setVolume.md), [play](play.md)
