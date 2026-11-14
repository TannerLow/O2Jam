# setStreamsVolume

[Index](../index.md)

Sets or gets the volume of all streams, or of all samples, at once. Covers `setStreamsVolume`, `setSamplesVolume`, `getStreamsVolume` and `getSamplesVolume`.

```cpp
void setStreamsVolume(float volume);
void setSamplesVolume(float volume);
float getStreamsVolume();
float getSamplesVolume();
```

`<o2/jam/audio.h>`, namespace `o2::jam`

## Parameters

| | |
|---|---|
| `volume` | From 0 (silent) to 1 (the files' own level). Values outside are clamped. |

## Return value

`getStreamsVolume` and `getSamplesVolume`: the volume last set, from 0 to 1. 1 before anything is set.

## Remarks

These multiply with each [stream](Stream.md)'s or [sample](Sample.md)'s own volume: a stream at volume 0.5 with streams volume 0.5 is heard at 0.25. They apply at once, also to sounds that are playing, and fit a game's music and effects volume sliders.

Both start at 1 in [init](init.md), and are kept across [setDevice](setDevice.md). They don't change the Windows volume. Setting them before `init` does nothing.

1 isn't amplified or dangerous: it's each file's own level, as loud as the same file in a media player at the same Windows volume. Every volume in the library only turns sound down. Songs are usually mastered loud, though, so a game should give its settings a lower starting value, and set these from the settings at startup. See [Volume](../concepts/streams-and-samples.md#volume).

## Example

```cpp
o2::jam::setStreamsVolume(settings.musicVolume);
o2::jam::setSamplesVolume(settings.effectsVolume);
```

## See also

[setVolume](setVolume.md), [Streams and samples](../concepts/streams-and-samples.md#volume)
