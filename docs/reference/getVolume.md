# getVolume

[Index](../index.md)

Gets the volume of a [Stream](Stream.md) or a [Sample](Sample.md).

```cpp
float getVolume(const Stream* stream);
float getVolume(const Sample* sample);
```

`<o2/jam/stream.h>`, `<o2/jam/sample.h>`, namespace `o2::jam`

## Parameters

| | |
|---|---|
| `stream`, `sample` | What to get the volume of. |

## Return value

The volume, from 0 (silent) to 1 (the file's own level). 0 if nothing is loaded.

## Remarks

**Streams:** the volume right now. During a [fade](fadeVolume.md), that's wherever the fade has got to, so reading it every frame shows the fade moving. After [fadeOutAndStop](fadeOutAndStop.md) it's 0.

**Samples:** the sample's own volume, set with [setVolume](setVolume.md) and used by [play](play.md)`(sample)`. The same as `sample->volume`. Playbacks started with their own volume, through `play(sample, volume, pan)`, don't change it.

Neither includes the global volume from [setStreamsVolume or setSamplesVolume](setStreamsVolume.md). The volume heard is this times that.

## Example

```cpp
// turn the song down by 10%
float volume = o2::jam::getVolume(&song);
o2::jam::setVolume(&song, volume * 0.9f);

// wait for a fade-in to finish
bool fadedIn = o2::jam::getVolume(&preview) >= 1.0f;
```

## See also

[setVolume](setVolume.md), [getPan](getPan.md), [setStreamsVolume](setStreamsVolume.md)
