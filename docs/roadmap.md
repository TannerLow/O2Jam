# Roadmap

[Index](index.md)

Planned, but left out of the first version to get something working first.

## Lower latency

The library uses BASS's default buffer sizes. Smaller ones (BASS's update period and device buffer settings) would make sounds start sooner, at the risk of crackling on slow machines. Worth measuring before changing. See [Latency](concepts/latency.md).

Further down: WASAPI exclusive mode or ASIO, through BASS's add-on libraries, which bypass Windows' mixer for the lowest latency possible.

## Rate for samples

[Samples](reference/Sample.md) always play at normal speed. Some games speed hitsounds up with the song in nightcore; that would be a rate per sample.

## Preview loop point

Looping a song from its preview point instead of its beginning, so games don't have to restart previews themselves. See [Song previews](concepts/previews.md#looping-the-preview).

## Lost devices

Telling the game when the device in use is unplugged, so it can switch to another. With [DEFAULT_DEVICE](reference/Device.md), Windows already moves output to the new default.
