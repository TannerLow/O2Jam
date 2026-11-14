# setLooping

[Index](../index.md)

Sets whether a [Stream](Stream.md) starts again from the beginning when it ends.

```cpp
bool setLooping(
    Stream* stream,
    bool looping
);
```

`<o2/jam/stream.h>`, namespace `o2::jam`

## Parameters

| | |
|---|---|
| `stream` | The stream. |
| `looping` | `true` to loop, `false` to stop at the end. |

## Return value

`true` if set. `false` if nothing is loaded.

## Remarks

The loop goes from the end of the file straight to its beginning with no gap, which suits menu music made to loop. It always loops the whole file; to loop a song preview from its preview point, see [Song previews](../concepts/previews.md#looping-the-preview).

Sets `stream->looping`. [load](load.md) turns looping off.

## Example

```cpp
o2::jam::load(&menuMusic, "menu.ogg");
o2::jam::setLooping(&menuMusic, true);
o2::jam::play(&menuMusic);
```

## See also

[isPlaying](isPlaying.md), [Stream](Stream.md)
