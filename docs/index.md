# O2Jam documentation

O2Jam is an audio library for rhythm games, written in C-style C++ on top of BASS. It plays streams (songs) and samples (hitsounds and other short sounds), changes a song's speed with or without changing its pitch, fades, and lets players choose their output device.

The docs come in two parts:

- **Concepts** explain the ideas behind the library. Read these first, in order. The reference pages assume them.
- **Reference** has one page per function or struct, all with the same layout: what it does, its declaration, parameters, return value, remarks, an example, and related pages.

## Concepts

1. [Getting started](concepts/getting-started.md): adding the library to a project, a minimal program, and errors
2. [Streams and samples](concepts/streams-and-samples.md): the two ways to play audio, and when to use which
3. [Rate and pitch](concepts/rate-and-pitch.md): double time, half time and nightcore, and what rate does to positions
4. [Song previews](concepts/previews.md): starting at a preview point, fading in and out, crossfading
5. [Output devices](concepts/devices.md): listing devices, the Default device, saving the player's choice, switching
6. [Latency](concepts/latency.md): why sound is heard late, what BASS measures, and using it as a hidden offset
7. [Memory, limits and threads](concepts/memory.md): what the library allocates (nothing), its fixed limits, and calling from one thread

The [roadmap](roadmap.md) lists what's planned but not done yet.

## Reference

### Setup (`<o2/jam/audio.h>`)

| | |
|---|---|
| [init](reference/init.md) | Starts audio output on a device; also `shutdown` |
| [Device](reference/Device.md) | An output device; also `listDevices`, `DEFAULT_DEVICE` |
| [setDevice](reference/setDevice.md) | Switches output to another device, moving everything loaded; also `getDevice` |
| [getLatency](reference/getLatency.md) | The delay before a played sound is output, as measured by BASS |
| [setStreamsVolume](reference/setStreamsVolume.md) | The volume of all streams; also `setSamplesVolume` |
| [getErrorMessage](reference/getErrorMessage.md) | Why the last call that failed, failed |
| [Limits](reference/limits.md) | How many streams and samples can be loaded at once |

### Streams and samples (`<o2/jam/stream.h>`, `<o2/jam/sample.h>`)

| | |
|---|---|
| [Stream](reference/Stream.md) | A loaded song or other long audio; also `RateMode` |
| [Sample](reference/Sample.md) | A loaded short sound that can play several times at once |
| [load](reference/load.md) | Loads a file into a Stream or Sample |
| [unload](reference/unload.md) | Frees a Stream or Sample |
| [play](reference/play.md) | Plays a stream, or starts a playback of a sample |
| [stop](reference/stop.md) | Stops a stream and goes back to the start, or stops all playbacks of a sample |
| [setVolume](reference/setVolume.md) | Sets the volume of a stream or sample |
| [setPan](reference/setPan.md) | Sets the pan of a stream or sample |

### Streams only (`<o2/jam/stream.h>`)

| | |
|---|---|
| [pause](reference/pause.md) | Pauses, keeping the position |
| [isPlaying](reference/isPlaying.md) | Whether it's playing; how to tell a song has ended |
| [seek](reference/seek.md) | Moves to a position in the song |
| [getPosition](reference/getPosition.md) | Where in the song it is |
| [setLooping](reference/setLooping.md) | Whether it starts again when it ends |
| [setRate](reference/setRate.md) | Changes the speed, with or without changing pitch |
| [fadeVolume](reference/fadeVolume.md) | Changes the volume smoothly over time |
| [fadeOutAndStop](reference/fadeOutAndStop.md) | Fades out, then stops |
