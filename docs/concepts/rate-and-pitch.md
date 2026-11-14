# Rate and pitch

[Index](../index.md) · Previous: [Streams and samples](streams-and-samples.md) · Next: [Song previews](previews.md)

[setRate](../reference/setRate.md) changes how fast a [stream](../reference/Stream.md) plays, from half speed (0.5) to double speed (2). Its [RateMode](../reference/Stream.md) decides what happens to the pitch:

| Mode | At 1.5× | Like |
|---|---|---|
| `RateMode::KeepPitch` | faster, same pitch | osu!'s double time and half time |
| `RateMode::ChangePitch` | faster and higher, like a record spun faster | osu!'s nightcore and daycore |

```cpp
// double time
o2::jam::setRate(&song, 1.5f, o2::jam::RateMode::KeepPitch);

// nightcore
o2::jam::setRate(&song, 1.5f, o2::jam::RateMode::ChangePitch);

// half time
o2::jam::setRate(&song, 0.75f, o2::jam::RateMode::KeepPitch);
```

The rate can change at any time, also while the stream plays. It stays through [stop](../reference/stop.md) and [seek](../reference/seek.md), but [load](../reference/load.md) sets it back to 1, so set it again after loading a new song.

## How it works

Every stream plays through BASS_FX's tempo processor.

- **ChangePitch** plays the audio back at a different sample rate: at 1.5×, the 44,100 samples of each second are played in 1/1.5 of a second. Speed and pitch change together, exactly as on a turntable. This costs nothing and sounds clean.
- **KeepPitch** time-stretches the audio: it cuts it into short overlapping pieces and spaces them closer together or further apart. This takes some processing, and strong changes can sound slightly smeared, especially on drums. That's the same trade-off every rhythm game makes for double time.

## Timing

Rate only changes how fast the song moves, not where things are in it. [getPosition](../reference/getPosition.md) and [seek](../reference/seek.md) use seconds of the song, as written in the chart: at 2×, the position advances by 2 seconds every real second, and [Stream](../reference/Stream.md)`::length` is always the length at rate 1. To know how long the song takes to play, divide by the rate.

A game that keeps its own clock in real time should multiply the time since the song started by the rate to get the song position. See [Latency](latency.md) for starting that clock at the right moment.

## Samples

[Samples](../reference/Sample.md) always play at normal speed and pitch, whatever streams' rates are. Hitsounds stay the same in double time, as players expect.
