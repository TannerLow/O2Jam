# load

[Index](../index.md)

Loads an audio file into a [Stream](Stream.md) or a [Sample](Sample.md).

```cpp
bool load(
    Stream* stream,
    const char* path
);

bool load(
    Sample* sample,
    const char* path,
    int maxPlaybacks
);
```

`<o2/jam/stream.h>`, `<o2/jam/sample.h>`, namespace `o2::jam`

## Parameters

| | |
|---|---|
| `stream`, `sample` | Where to load it. Anything already loaded there is unloaded first. |
| `path` | The file, as a UTF-8 path, absolute or relative to the working directory. Characters outside ASCII, like Japanese folder names, work. |
| `maxPlaybacks` | Samples only: how many times the sample can play at once, from 1 to 65535. |

## Return value

`true` if the file was loaded. `false` if it can't be opened, its format isn't supported, the [limit](limits.md) of loaded streams or samples is reached, or the library isn't [started](init.md). On failure the Stream or Sample is left unloaded, with `handle` 0.

## Remarks

**Streams** open the file and keep it open while loaded, reading as they play. Loading scans the whole file once so that [seek](seek.md) is exact and `length` is right, even for MP3s. The stream starts stopped at the beginning, with volume 1, pan 0, rate 1 (`KeepPitch`) and looping off.

**Samples** read and decode the whole file into memory, and close it. That takes longer and uses more memory than a stream, about 11 MB per minute of 48 kHz stereo, but makes [play](play.md) instant. Volume starts at 1 and pan at 0.

Loading reads from disk and can be slow; do it while loading a chart or a screen, not during gameplay.

Supported formats are MP3, OGG Vorbis, WAV and AIFF, and others Windows has codecs for, like AAC and WMA.

Paths longer than 1023 characters fail.

Calling `load` before [init](init.md), with a null `path`, or with `maxPlaybacks` out of range asserts in Debug builds, and returns `false` in Release. So does loading past the [limits](limits.md).

## Example

```cpp
o2::jam::Stream song = {};
if (!o2::jam::load(&song, "songs/夜に駆ける/audio.ogg")) {
	log("%s", o2::jam::getErrorMessage());
}

o2::jam::Sample keysounds[256] = {};
for (int i = 0; i < chart.soundCount; i++) {
	o2::jam::load(&keysounds[i], chart.soundPaths[i], 2);
}
```

## See also

[unload](unload.md), [Stream](Stream.md), [Sample](Sample.md), [Streams and samples](../concepts/streams-and-samples.md)
