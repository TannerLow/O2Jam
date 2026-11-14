# setStreamsVolume

[Index](../index.md)

Sets the volume of all streams, or of all samples, at once. Covers `setStreamsVolume` and `setSamplesVolume`.

```cpp
void setStreamsVolume(float volume);
void setSamplesVolume(float volume);
```

`<o2/jam/audio.h>`, namespace `o2::jam`

## Parameters

| | |
|---|---|
| `volume` | From 0 (silent) to 1 (full). Values outside are clamped. |

## Remarks

These multiply with each [stream](Stream.md)'s or [sample](Sample.md)'s own volume: a stream at volume 0.5 with streams volume 0.5 is heard at 0.25. They apply at once, also to sounds that are playing, and fit a game's music and effects volume sliders.

Both start at 1 in [init](init.md), and are kept across [setDevice](setDevice.md). They don't change the Windows volume.

Before `init`, they do nothing.

## Example

```cpp
o2::jam::setStreamsVolume(settings.musicVolume);
o2::jam::setSamplesVolume(settings.effectsVolume);
```

## See also

[setVolume](setVolume.md), [Streams and samples](../concepts/streams-and-samples.md#volume)
