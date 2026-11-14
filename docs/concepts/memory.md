# Memory, limits and threads

[Index](../index.md) · Previous: [Latency](latency.md)

## Memory

O2Jam doesn't call `malloc` or `new`, and doesn't use standard library containers.

- [Stream](../reference/Stream.md) and [Sample](../reference/Sample.md) are small structs you own: on the stack, in an array, in your own arena. They hold a BASS handle and a few settings, not the audio.
- The library's own state is a few fixed arrays, sized by [limits](../reference/limits.md) (about 16 KB with the defaults).
- BASS allocates memory for the audio itself, inside BASS: a buffer for each stream, and the whole decoded sound for each sample. It's freed by [unload](../reference/unload.md) or [shutdown](../reference/init.md).

Nothing allocates during gameplay: [play](../reference/play.md), volume, pan and position calls only change state that's already there.

## Limits

The library remembers the handle of every loaded stream and sample, so that [setDevice](../reference/setDevice.md) can move them all to a new device. These lists have fixed sizes:

| Macro | Default | Limits |
|---|---|---|
| `O2_JAM_MAX_STREAMS` | 64 | Streams loaded at once |
| `O2_JAM_MAX_SAMPLES` | 4096 | Samples loaded at once, enough for keysounded charts with a sound for every note |

Loading one more fails, and [getErrorMessage](../reference/getErrorMessage.md) says the limit was hit. See [limits](../reference/limits.md) to change them.

## Threads

Call everything from one thread, normally the game's main thread. The library's own lists aren't protected against two threads changing them at once. BASS plays the audio on its own threads, so nothing has to be called regularly to keep sound going: a game can call nothing for seconds and the song plays on.
