# getErrorMessage

[Index](../index.md)

Says why the last call that failed, failed.

```cpp
const char* getErrorMessage();
```

`<o2/jam/audio.h>`, namespace `o2::jam`

## Return value

A short English description, such as `"Can't open the file (BASS error 2)"` or `"Too many samples loaded (O2_JAM_MAX_SAMPLES is 4096)"`. Never null. The string is static: don't free it.

## Remarks

Every function that returns `bool` sets the message when it returns `false`. Calls that succeed don't clear it, so read it straight after the call that failed.

Messages from BASS include BASS's error code, which the BASS documentation (`externals/BASS/bass.chm`) explains further.

The message belongs to the thread that made the call. Since the library is used from one thread, that's the game's main thread.

## Example

```cpp
if (!o2::jam::load(&song, path)) {
	log("Can't load %s: %s", path, o2::jam::getErrorMessage());
}
```

## See also

[Getting started](../concepts/getting-started.md#errors)
