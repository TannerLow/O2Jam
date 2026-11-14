# fadeOutAndStop

[Index](../index.md)

Fades a [Stream](Stream.md) out, then stops it.

```cpp
bool fadeOutAndStop(
    Stream* stream,
    float seconds
);
```

`<o2/jam/stream.h>`, namespace `o2::jam`

## Parameters

| | |
|---|---|
| `stream` | The stream. |
| `seconds` | How long the fade takes. 0 or less stops at once. |

## Return value

`true` if the fade started. `false` if nothing is loaded.

## Remarks

Returns immediately. When the volume reaches 0, the stream stops as [stop](stop.md) does, moving back to the beginning, and [isPlaying](isPlaying.md) becomes `false`. A stream that isn't playing stops at once.

The volume stays at 0 afterwards, so set it with [setVolume](setVolume.md), or fade it in with [fadeVolume](fadeVolume.md), before playing the stream again. [load](load.md) also sets it back to 1.

Calling [play](play.md) during the fade doesn't cancel it; [setVolume](setVolume.md) or [fadeVolume](fadeVolume.md) does.

## Example

```cpp
// leaving the song select screen
o2::jam::fadeOutAndStop(&preview, 0.3f);
```

## See also

[fadeVolume](fadeVolume.md), [stop](stop.md), [Song previews](../concepts/previews.md)
