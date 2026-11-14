# Roadmap

[Index](index.md)

Planned, but left out of the first version to get something working first.

## Lower latency

The library uses BASS's default buffer sizes. Smaller ones (BASS's update period and device buffer settings) would make sounds start sooner, at the risk of crackling on slow machines. Turning off a stream's own playback buffer (BASS's `BASS_ATTRIB_BUFFER` set to 0) has the stream produce audio only as the device needs it, which BASS documents as the lowest-latency option. Worth measuring before changing. See [Latency](concepts/latency.md).

With smaller buffers, BASS_FX's time-stretching has to finish within each short mixing step, or every sound on the device skips. Its faster, lower-quality mode (`BASS_ATTRIB_TEMPO_OPTION_USE_QUICKALGO`, which the old O2Jam used) is worth trying then. It saves CPU but doesn't itself lower latency; the library uses full quality for now.

Further down: WASAPI exclusive mode or ASIO, through BASS's add-on libraries, which bypass Windows' mixer for the lowest latency possible.

## Rate for samples

[Samples](reference/Sample.md) always play at normal speed. Some games speed hitsounds up with the song in nightcore; that would be a rate per sample.

## Preview loop point

Looping a song from its preview point instead of its beginning, so games don't have to restart previews themselves. See [Song previews](concepts/previews.md#looping-the-preview).

## Lost devices

Telling the game when the device in use is unplugged, so it can switch to another. With [DEFAULT_DEVICE](reference/Device.md), Windows already moves output to the new default.
