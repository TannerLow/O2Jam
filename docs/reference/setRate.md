# setRate

[Index](../index.md)

Changes how fast a [Stream](Stream.md) plays, with or without changing its pitch.

```cpp
bool setRate(
    Stream* stream,
    float rate,
    RateMode mode
);
```

`<o2/jam/stream.h>`, namespace `o2::jam`

## Parameters

| | |
|---|---|
| `stream` | The stream. |
| `rate` | The speed, from 0.5 (half) to 2 (double). 1 is normal. |
| `mode` | `RateMode::KeepPitch` to change only the speed (double time), or `RateMode::ChangePitch` for pitch to follow the speed (nightcore). |

## Return value

`true` if set. `false` if `rate` is outside 0.5 to 2, or nothing is loaded; the rate is then unchanged.

## Remarks

Takes effect immediately, also while playing, and stays through [stop](stop.md), [pause](pause.md) and [seek](seek.md). [load](load.md) sets it back to 1. Sets `stream->rate` and `stream->rateMode`.

`ChangePitch` is a plain change of playback speed and costs nothing. `KeepPitch` time-stretches the audio, which takes a little processing; strong changes can sound slightly smeared, as in every game with double time. See [Rate and pitch](../concepts/rate-and-pitch.md).

Positions stay in seconds of the song: [getPosition](getPosition.md), [seek](seek.md) and `length` don't change meaning with the rate.

[Samples](Sample.md) aren't affected.

## Example

```cpp
switch (mods.speed) {
case SpeedMod::None:       o2::jam::setRate(&song, 1.0f, o2::jam::RateMode::KeepPitch); break;
case SpeedMod::DoubleTime: o2::jam::setRate(&song, 1.5f, o2::jam::RateMode::KeepPitch); break;
case SpeedMod::Nightcore:  o2::jam::setRate(&song, 1.5f, o2::jam::RateMode::ChangePitch); break;
case SpeedMod::HalfTime:   o2::jam::setRate(&song, 0.75f, o2::jam::RateMode::KeepPitch); break;
}
```

## See also

[Stream](Stream.md), [Rate and pitch](../concepts/rate-and-pitch.md)
