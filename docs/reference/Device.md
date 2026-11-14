# Device

[Index](../index.md)

An output device, and the function that lists them. Covers `Device`, `listDevices` and `DEFAULT_DEVICE`.

```cpp
const int DEFAULT_DEVICE = -1;

struct Device {
    int index;
    char name[256];
    char driver[256];
    bool isDefault;
    bool isEnabled;
};

int listDevices(
    Device* devices,
    int capacity
);
```

`<o2/jam/audio.h>`, namespace `o2::jam`

## Members

| | |
|---|---|
| `index` | The number to pass to [init](init.md) or [setDevice](setDevice.md). Can change between runs. |
| `name` | The device's name as Windows shows it, for showing to the player. UTF-8, null-terminated, cut off if longer than 255 bytes. |
| `driver` | An identifier for the device that stays the same between runs. Save this in settings, not `index`. |
| `isDefault` | Whether this is the device Windows currently uses by default. |
| `isEnabled` | `false` if the device is unplugged or disabled. Such a device is listed, but [init](init.md) and [setDevice](setDevice.md) fail on it. |

## Parameters

| | |
|---|---|
| `devices` | An array to fill. Can be null if `capacity` is 0, to only count the devices. |
| `capacity` | How many Devices fit in `devices`. |

## Return value

How many output devices there are, which can be more than `capacity`. Only the first `capacity` are written.

## Remarks

`listDevices` works before [init](init.md), so a game can offer the choice of device before starting audio.

`DEFAULT_DEVICE` isn't an entry in the list but a choice: the device Windows uses by default, followed automatically when the player changes it in Windows. On Windows, the first entry in the list, named "Default", is the same choice, and [getDevice](setDevice.md) reports it as `DEFAULT_DEVICE`.

Listing asks Windows about every device, so don't call it every frame; once when the settings screen opens is enough.

## Example

```cpp
o2::jam::Device devices[32];
int count = o2::jam::listDevices(devices, 32);
for (int i = 0; i < count && i < 32; i++) {
	printf("%d: %s%s\n", devices[i].index, devices[i].name, devices[i].isEnabled ? "" : " (unplugged)");
}
```

## See also

[setDevice](setDevice.md), [init](init.md), [Output devices](../concepts/devices.md)
