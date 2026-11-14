# Output devices

[Index](../index.md) · Previous: [Song previews](previews.md) · Next: [Latency](latency.md)

Audio plays on one output device at a time: speakers, headphones, a USB interface. Players choose it in the game's settings.

## Listing devices

[listDevices](../reference/Device.md) fills an array of [Device](../reference/Device.md)s. It works before [init](../reference/init.md), so a game can show the choice before starting audio.

```cpp
o2::jam::Device devices[32];
int count = o2::jam::listDevices(devices, 32);
if (count > 32) {
	count = 32; // more devices than fit; ignore the rest
}
for (int i = 0; i < count; i++) {
	if (devices[i].isEnabled) {
		addToDropdown(devices[i].name); // UTF-8
	}
}
```

The list includes devices that are unplugged or disabled, with `isEnabled` false, so a device a player picked before still shows up while it's unplugged. Such a device can't be used.

## The Default device

[DEFAULT_DEVICE](../reference/Device.md) isn't a device, but follows whichever device Windows uses: when the player switches Windows to headphones, the game switches too, without restarting. It's the best choice unless the player picks something else. The first entry in the list, named "Default", is this same choice.

## Saving the choice

A device's `index` can change between runs, when devices are plugged in or removed. Save its `driver` string in the settings file instead, and look it up at startup:

```cpp
int findDevice(const char* savedDriver) {
	o2::jam::Device devices[32];
	int count = o2::jam::listDevices(devices, 32);
	for (int i = 0; i < count && i < 32; i++) {
		if (strcmp(devices[i].driver, savedDriver) == 0 && devices[i].isEnabled) {
			return devices[i].index;
		}
	}
	return o2::jam::DEFAULT_DEVICE; // gone: fall back
}

o2::jam::init(findDevice(settings.audioDevice));
```

## Switching devices

[setDevice](../reference/setDevice.md) switches while the game runs. Every loaded [stream](../reference/Stream.md) and [sample](../reference/Sample.md) moves to the new device, and a song that's playing keeps playing from where it was, after a short gap. Switching is too slow to do during gameplay; do it from the settings screen.

The library keeps the handle of every loaded stream and sample in fixed arrays to be able to move them, which is what limits how many can be loaded at once. See [Memory, limits and threads](memory.md).

[getLatency](../reference/getLatency.md) is measured again for the new device.
