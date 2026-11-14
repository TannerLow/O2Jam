# isLoaded

[Index](../index.md)

Tells whether a file is loaded into a [Stream](Stream.md) or a [Sample](Sample.md).

```cpp
bool isLoaded(const Stream* stream);
bool isLoaded(const Sample* sample);
```

`<o2/jam/stream.h>`, `<o2/jam/sample.h>`, namespace `o2::jam`

## Parameters

| | |
|---|---|
| `stream`, `sample` | What to check. |

## Return value

`true` after a successful [load](load.md), until [unload](unload.md). `false` for a zero-initialized struct, after a failed load, and after unload.

## Remarks

The same as checking `handle != 0`, but says what's meant.

After [shutdown](init.md), structs that were loaded still report `true`, because the library doesn't know where they are to clear them. Don't use them after shutdown.

## Example

```cpp
if (!o2::jam::isLoaded(&preview)) {
	drawText("No preview");
}
```

## See also

[load](load.md), [unload](unload.md)
