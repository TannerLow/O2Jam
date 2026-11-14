# unload

[Index](../index.md)

Frees a loaded [Stream](Stream.md) or [Sample](Sample.md).

```cpp
void unload(Stream* stream);
void unload(Sample* sample);
```

`<o2/jam/stream.h>`, `<o2/jam/sample.h>`, namespace `o2::jam`

## Parameters

| | |
|---|---|
| `stream`, `sample` | What to unload. |

## Remarks

Stops all sound from it, frees what BASS holds for it (the open file for a stream, the decoded sound for a sample), and sets `handle` to 0. Other fields are left as they were.

Does nothing if nothing is loaded, so it's safe to call on a zero-initialized or already unloaded struct.

[load](load.md) into a loaded struct unloads it first, so there's no need to unload before loading the next song into the same Stream.

## Example

```cpp
// leaving a chart
for (int i = 0; i < chart.soundCount; i++) {
	o2::jam::unload(&keysounds[i]);
}
o2::jam::unload(&song);
```

## See also

[load](load.md), [shutdown](init.md)
