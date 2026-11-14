# Latency

[Index](../index.md) · Previous: [Output devices](devices.md) · Next: [Memory, limits and threads](memory.md)

Latency is the time between asking for a sound and hearing it. Players judge timing by ear, so it matters: if the song is heard 40 ms later than the game thinks it started, every note looks early.

## Where it comes from

Sound doesn't go straight from a [play](../reference/play.md) call to the speakers. BASS mixes everything that's playing into a buffer, and Windows plays that buffer on the device. A sound started now is heard once the audio already queued ahead of it has played. With BASS's default settings on Windows, that's typically 20 to 60 ms, and it's about the same for every sound on a device.

Hardware after that adds more that no software can see: Bluetooth headphones add 100 to 200 ms, and some TVs and USB devices add their own.

## The measured latency

BASS measures the first part, the average delay before the device starts playing a new sound, when it starts a device. [getLatency](../reference/getLatency.md) returns it in seconds. It's measured again by [setDevice](../reference/setDevice.md).

A game whose clock runs on real time can use it as a hidden offset: the song started when [play](../reference/play.md) returned, but is heard `getLatency()` seconds later.

```cpp
o2::jam::play(&song);
double songStart = now() + o2::jam::getLatency();

// every frame
double songTime = (now() - songStart) * song.rate - settings.audioOffset;
```

The audio offset setting in the game covers what BASS can't measure, like Bluetooth, and personal taste. With the measured latency already accounted for, most players can leave it at 0.

Hitsounds have the same latency: one is heard about `getLatency()` after the key press. No offset can make it earlier; only lower latency helps, below.

## Keeping it low

The library uses BASS's default buffer settings for now. Smaller buffers would lower latency further, at the risk of crackling on slower machines. That and WASAPI exclusive mode, which bypasses Windows' mixer, are on the [roadmap](../roadmap.md).
