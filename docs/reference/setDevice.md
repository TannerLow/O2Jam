# setDevice

[Index](../index.md)

Switches audio output to another device while the game runs, and tells which device is in use. Covers `setDevice` and `getDevice`.

```cpp
bool setDevice(int deviceIndex);
int getDevice();
```

`<o2/jam/audio.h>`, namespace `o2::jam`

## Parameters

| | |
|---|---|
| `deviceIndex` | [DEFAULT_DEVICE](Device.md), or the `index` of a [Device](Device.md) from [listDevices](Device.md). |

## Return value

`setDevice`: `true` if output is now on the device. `false` if the new device couldn't be started; output then stays on the old device, with nothing changed. `false` too if the library isn't [started](init.md).

`getDevice`: the device in use, as passed to [init](init.md) or `setDevice`: `DEFAULT_DEVICE` or a device's `index`. Before `init` it returns `DEFAULT_DEVICE`, which is what most games pass to `init` anyway.

## Remarks

Every loaded [stream](Stream.md) and [sample](Sample.md) moves to the new device. Streams keep their position, volume, pan, rate and whether they're playing, so a song carries on from where it was after a short gap. Sample playbacks that are going are moved too.

The old device is closed afterwards. [getLatency](getLatency.md) is measured again for the new device.

Switching takes a moment, so do it from a settings screen, not during gameplay. Switching to the device already in use does nothing and returns `true`.

## Example

```cpp
// the player picked a device in the settings dropdown
if (o2::jam::setDevice(devices[picked].index)) {
	strcpy(settings.audioDevice, devices[picked].driver);
} else {
	showError(o2::jam::getErrorMessage());
}
```

## See also

[Device](Device.md), [init](init.md), [getLatency](getLatency.md), [Output devices](../concepts/devices.md)
