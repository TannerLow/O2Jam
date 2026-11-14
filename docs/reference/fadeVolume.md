# fadeVolume

[Index](../index.md)

Changes the volume of a [Stream](Stream.md) smoothly over time.

```cpp
bool fadeVolume(
    Stream* stream,
    float volume,
    float seconds
);
```

`<o2/jam/stream.h>`, namespace `o2::jam`

## Parameters

| | |
|---|---|
| `stream` | The stream. |
| `volume` | The volume to end at, from 0 to 1. Clamped. |
| `seconds` | How long the fade takes. 0 or less sets the volume at once. |

## Return value

`true` if the fade started. `false` if nothing is loaded.

## Remarks

Returns immediately; BASS changes the volume as it mixes, so the fade is smooth and nothing needs to be called while it runs. The fade doesn't delay the sound: a stream played at volume 0 and faded in is heard from the start, getting louder.

A new `fadeVolume` replaces a fade in progress, starting from wherever the volume has got to. [setVolume](setVolume.md) and [stop](stop.md) cancel it. The fade runs while the stream plays and waits while it's [paused](pause.md).

## Example

```cpp
// fade in a song preview
o2::jam::setVolume(&preview, 0.0f);
o2::jam::play(&preview);
o2::jam::fadeVolume(&preview, 1.0f, 0.5f);
```

## See also

[fadeOutAndStop](fadeOutAndStop.md), [setVolume](setVolume.md), [Song previews](../concepts/previews.md)
