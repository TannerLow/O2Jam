# setVolume

[Index](../index.md)

Sets the volume of a [Stream](Stream.md), or of later playbacks of a [Sample](Sample.md).

```cpp
bool setVolume(
    Stream* stream,
    float volume
);

bool setVolume(
    Sample* sample,
    float volume
);
```

`<o2/jam/stream.h>`, `<o2/jam/sample.h>`, namespace `o2::jam`

## Parameters

| | |
|---|---|
| `stream`, `sample` | What to set the volume of. |
| `volume` | From 0 (silent) to 1 (the file's own level). Values outside are clamped. |

## Return value

`true` if set. `false` if nothing is loaded.

## Remarks

**Streams** change at once, also while playing. A [fade](fadeVolume.md) in progress is cancelled.

**Samples** store it in `sample->volume` for later [play](play.md)`(sample)` calls. Playbacks already going keep the volume they started with.

The volume heard is also multiplied by the global volume for streams or samples, from [setStreamsVolume](setStreamsVolume.md). For a game's volume sliders, use those; use this for the balance between individual sounds.

## Example

```cpp
o2::jam::setVolume(&song, 0.8f);
o2::jam::setVolume(&hit, 0.5f);
```

## See also

[getVolume](getVolume.md), [setPan](setPan.md), [fadeVolume](fadeVolume.md), [setStreamsVolume](setStreamsVolume.md)
