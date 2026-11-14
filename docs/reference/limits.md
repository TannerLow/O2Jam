# Limits

[Index](../index.md)

Macros that set how many streams and samples can be loaded at once.

```cpp
#define O2_JAM_MAX_STREAMS 64
#define O2_JAM_MAX_SAMPLES 4096
```

`<o2/jam/audio.h>`

## Values

| | Default | |
|---|---|---|
| `O2_JAM_MAX_STREAMS` | 64 | [Streams](Stream.md) loaded at once. |
| `O2_JAM_MAX_SAMPLES` | 4096 | [Samples](Sample.md) loaded at once. |

## Remarks

The library keeps the BASS handle of every loaded stream and sample, 4 to 8 bytes each, so [setDevice](setDevice.md) can move them and [shutdown](init.md) can free them. [load](load.md) fails when the list is full, and [getErrorMessage](getErrorMessage.md) names the macro.

Each is defined only if it isn't already, so you can override them. Define them for the whole project (in Visual Studio: Project Properties → C/C++ → Preprocessor → Preprocessor Definitions, e.g. `O2_JAM_MAX_SAMPLES=8192`), so the library's `.cpp` files see the same value as yours.

## See also

[Memory, limits and threads](../concepts/memory.md)
