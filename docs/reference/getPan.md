# getPan

[Index](../index.md)

Gets the pan of a [Stream](Stream.md) or a [Sample](Sample.md).

```cpp
float getPan(const Stream* stream);
float getPan(const Sample* sample);
```

`<o2/jam/stream.h>`, `<o2/jam/sample.h>`, namespace `o2::jam`

## Parameters

| | |
|---|---|
| `stream`, `sample` | What to get the pan of. |

## Return value

The pan, from -1 (all left) through 0 (center) to 1 (all right). 0 if nothing is loaded.

## Remarks

**Streams:** the stream's pan, set with [setPan](setPan.md).

**Samples:** the sample's own pan, set with [setPan](setPan.md) and used by [play](play.md)`(sample)`. The same as `sample->pan`. Playbacks started with their own pan don't change it.

## Example

```cpp
float pan = o2::jam::getPan(&song);
```

## See also

[setPan](setPan.md), [getVolume](getVolume.md)
