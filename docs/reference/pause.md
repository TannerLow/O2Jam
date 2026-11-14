# pause

[Index](../index.md)

Pauses a [Stream](Stream.md), keeping its position.

```cpp
bool pause(Stream* stream);
```

`<o2/jam/stream.h>`, namespace `o2::jam`

## Parameters

| | |
|---|---|
| `stream` | The stream to pause. |

## Return value

`true` if the stream is now paused or wasn't playing. `false` if nothing is loaded.

## Remarks

[play](play.md) resumes from the same position. Volume, pan and rate are kept.

A fade in progress pauses with the stream and continues when it resumes.

## Example

```cpp
// the player pressed Escape
o2::jam::pause(&song);
showPauseMenu();
```

## See also

[play](play.md), [stop](stop.md)
