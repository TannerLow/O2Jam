# Sample

[Index](../index.md)

A loaded sample: a short sound, decoded into memory, that can play several times at once.

```cpp
struct Sample {
    uint32_t handle;
    int maxPlaybacks;
    float volume;
    float pan;
};
```

`<o2/jam/sample.h>`, namespace `o2::jam`

## Members

| | |
|---|---|
| `handle` | The BASS handle, or 0 when nothing is loaded. |
| `maxPlaybacks` | How many times it can play at once, as given to [load](load.md). |
| `volume` | The volume [play](play.md)`(sample)` uses, set with [setVolume](setVolume.md). 1 after load. |
| `pan` | The pan [play](play.md)`(sample)` uses, set with [setPan](setPan.md). 0 after load. |

## Remarks

A Sample is a plain struct you own. The decoded sound lives inside BASS until [unload](unload.md).

Read the fields freely, but change them only through the functions. In Debug builds, passing a Sample whose `handle` the library doesn't know asserts, as for a [Stream](Stream.md). Zero-initialize a Sample (`= {}`) before its first [load](load.md). [isLoaded](isLoaded.md) tells whether one is loaded.

Samples always play at normal speed and pitch: a stream's [rate](setRate.md) doesn't affect them.

## Example

```cpp
o2::jam::Sample hit = {};
o2::jam::load(&hit, "skin/hit.wav", 8);
o2::jam::setVolume(&hit, 0.6f);

// on every hit
o2::jam::play(&hit);
```

## See also

[load](load.md), [play](play.md), [Stream](Stream.md), [Streams and samples](../concepts/streams-and-samples.md)
