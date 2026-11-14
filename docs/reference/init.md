# init

[Index](../index.md)

Starts and stops the library. Covers `init` and `shutdown`.

```cpp
bool init(int deviceIndex);
void shutdown();
```

`<o2/jam/audio.h>`, namespace `o2::jam`

## Parameters

| | |
|---|---|
| `deviceIndex` | The output device: [DEFAULT_DEVICE](Device.md) to follow Windows' default device, or the `index` of a [Device](Device.md) from [listDevices](Device.md). |

## Return value

`init`: `true` if audio output started. `false` if the device couldn't be started, for example because it's unplugged or in use, or if the library is already started. [getErrorMessage](getErrorMessage.md) says which.

## Remarks

Call `init` before any other function except [listDevices](Device.md). It starts BASS on the device and measures the device's [latency](getLatency.md), which can take a moment, so call it during startup, not during gameplay.

If the chosen device fails, `init` doesn't try another. A game can fall back to `DEFAULT_DEVICE` itself, as in the example.

Calling `init` again without `shutdown` asserts in Debug builds, and returns `false` in Release.

`shutdown` stops all sound and frees every loaded [stream](Stream.md) and [sample](Sample.md), including the memory BASS holds for them. Stream and Sample structs loaded before are left as they were, with handles that no longer mean anything. Don't use them afterwards; to reuse one after another `init`, zero it (`= {}`) first. Using one without zeroing asserts in Debug builds. [Unloading](unload.md) everything first is cleaner but not needed. `shutdown` does nothing if the library isn't started.

## Example

```cpp
if (!o2::jam::init(savedDevice)) {
	if (!o2::jam::init(o2::jam::DEFAULT_DEVICE)) {
		showError(o2::jam::getErrorMessage());
	}
}

/* ... the game ... */

o2::jam::shutdown();
```

## See also

[Device](Device.md), [setDevice](setDevice.md), [getLatency](getLatency.md), [Getting started](../concepts/getting-started.md)
