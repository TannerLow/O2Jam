# stop

[Index](../index.md)

Stops a [Stream](Stream.md) and moves it back to the beginning, or stops every playback of a [Sample](Sample.md).

```cpp
bool stop(Stream* stream);
bool stop(Sample* sample);
```

`<o2/jam/stream.h>`, `<o2/jam/sample.h>`, namespace `o2::jam`

## Parameters

| | |
|---|---|
| `stream`, `sample` | What to stop. |

## Return value

`true` if it's stopped. `false` if nothing is loaded.

## Remarks

A stopped stream plays from the beginning on the next [play](play.md). To stop and keep the position, use [pause](pause.md). Volume, pan, rate and looping are kept. A fade in progress is cancelled, leaving the volume where the fade had got to.

For a sample, every playback that's going stops at once. The sample stays loaded.

## Example

```cpp
// restarting a chart
o2::jam::stop(&song);
for (int i = 0; i < chart.soundCount; i++) {
	o2::jam::stop(&keysounds[i]);
}
o2::jam::play(&song);
```

## See also

[pause](pause.md), [play](play.md), [fadeOutAndStop](fadeOutAndStop.md)
