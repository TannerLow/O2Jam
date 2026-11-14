# getLatency

[Index](../index.md)

How long it takes the device to start outputting a sound after it's played, as measured by BASS.

```cpp
float getLatency();
```

`<o2/jam/audio.h>`, namespace `o2::jam`

## Return value

The latency in seconds, for example 0.03 for 30 ms. 0 if the library isn't [started](init.md).

## Remarks

BASS measures it when it starts a device, in [init](init.md) and [setDevice](setDevice.md); this function only returns the stored value, so it's cheap to call.

It's an average, and covers BASS's and Windows' buffering. It can't include delay added after the computer, such as by Bluetooth headphones, which the game's audio offset setting is for. See [Latency](../concepts/latency.md).

## Example

```cpp
o2::jam::play(&song);
songStartTime = now() + o2::jam::getLatency();
```

## See also

[play](play.md), [setDevice](setDevice.md), [Latency](../concepts/latency.md)
