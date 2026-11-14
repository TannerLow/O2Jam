# Getting started

[Index](../index.md) · Next: [Streams and samples](streams-and-samples.md)

## Files

| File | What it is |
|---|---|
| `include/o2/jam/audio.h`, `src/o2/jam/audio.cpp` | Starting and stopping the library, output devices, latency, global volumes, errors. |
| `include/o2/jam/stream.h`, `src/o2/jam/stream.cpp` | [Streams](../reference/Stream.md): songs. Play, pause, seek, loop, fade, and change speed with or without changing pitch. |
| `include/o2/jam/sample.h`, `src/o2/jam/sample.cpp` | [Samples](../reference/Sample.md): hitsounds and other short sounds that can overlap. |

To use the library in a Visual Studio project:

1. Put O2Jam's `include` folder and `externals/BASS/include` on the include path.
2. Add the `.cpp` files to the project.
3. Link `bass.lib` and `bass_fx.lib` from `externals/BASS/lib/x64` (or `x86` for 32-bit builds).
4. Copy `bass.dll` and `bass_fx.dll` from the same folder next to the executable. The `.lib` files only point to the DLLs, so the program won't start without them.
5. If the project also compiles another o2 library's sources, set Object File Name to `$(IntDir)%(RelativeDir)` (Project Properties → C/C++ → Output Files), so `.cpp` files with the same name in different libraries don't overwrite each other's object files.

Include headers as `#include <o2/jam/audio.h>`. Everything is in the `o2::jam` namespace.

## A minimal program

```cpp
#include <stdio.h>
#include <o2/jam/audio.h>
#include <o2/jam/stream.h>
#include <o2/jam/sample.h>

int main() {
	// 1. Start audio on the device Windows uses
	if (!o2::jam::init(o2::jam::DEFAULT_DEVICE)) {
		printf("No audio: %s\n", o2::jam::getErrorMessage());
		return 1;
	}

	// 2. Load a song and a hitsound. Paths are UTF-8
	o2::jam::Stream song = {};
	o2::jam::Sample hit = {};
	if (!o2::jam::load(&song, "songs/曲/audio.mp3")) {
		printf("Can't load the song: %s\n", o2::jam::getErrorMessage());
	}
	o2::jam::load(&hit, "skin/hit.wav", 8);

	// 3. Play the song at 1.5x speed, keeping its pitch (double time)
	o2::jam::setRate(&song, 1.5f, o2::jam::RateMode::KeepPitch);
	o2::jam::play(&song);

	// 4. During the game, play the hitsound whenever a note is hit
	o2::jam::play(&hit);

	// 5. Clean up
	o2::jam::unload(&hit);
	o2::jam::unload(&song);
	o2::jam::shutdown();
	return 0;
}
```

The demo in `O2Jam/main.cpp` is a longer example: a console program that plays `resources/example_song.mp3` and `resources/example_sample.wav`, with keys for play, pause, stop, seek, looping, double time, nightcore and half time, volume, panned hitsounds, a fading preview, and switching output devices. Run it from the `O2Jam` project folder, which is Visual Studio's default working directory, so it finds `resources/`.

## Errors

Functions that can fail return `bool`: `true` on success. When one returns `false`, [getErrorMessage](../reference/getErrorMessage.md) says why, for example that a file couldn't be opened or its format isn't supported. Nothing is printed, and the library never ends the program.

Calling a function on a [Stream](../reference/Stream.md) or [Sample](../reference/Sample.md) that isn't loaded returns `false` and does nothing else. Zero-initialize them (`= {}`) so that's always the case before [load](../reference/load.md).

### Asserts

Mistakes in how the library is called, as opposed to things going wrong at run time, stop a Debug build with an `assert`, so they're found where they happen:

- calling [load](../reference/load.md) or [setDevice](../reference/setDevice.md) before [init](../reference/init.md), or `init` twice
- null pointers, and a null `devices` with a non-zero capacity in [listDevices](../reference/Device.md)
- a rate outside 0.5 to 2 in [setRate](../reference/setRate.md), or `maxPlaybacks` outside 1 to 65535
- a Stream or Sample that wasn't zero-initialized, was copied and then unloaded through the copy, or was used after [shutdown](../reference/init.md)
- loading more than the [limits](../reference/limits.md) allow

In Release builds `assert` is compiled out, and the same calls return `false` with a message from [getErrorMessage](../reference/getErrorMessage.md), as described on each page. Things that can go wrong at run time never assert: a missing file, an unsupported format, an unplugged device, seeking past the end, or calling a function on a stream that isn't loaded, which is what a failed load leaves.
