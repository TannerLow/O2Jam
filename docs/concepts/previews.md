# Song previews

[Index](../index.md) · Previous: [Rate and pitch](rate-and-pitch.md) · Next: [Output devices](devices.md)

On a song select screen, the selected song plays from its preview point, usually the chorus, fading in, and fades out when the player moves on. That's a [Stream](../reference/Stream.md) with [seek](../reference/seek.md) and the two fade functions.

```cpp
o2::jam::Stream preview = {};

// on selecting a song: startPreview(&preview, path, previewTime);

void startPreview(
	o2::jam::Stream* stream,
	const char* path,
	double previewTime
) {
	// load unloads the previous song first
	if (!o2::jam::load(stream, path)) {
		return;
	}
	o2::jam::seek(stream, previewTime);
	o2::jam::setVolume(stream, 0.0f);
	o2::jam::play(stream);
	o2::jam::fadeVolume(stream, 1.0f, 0.5f); // fade in over half a second
}

void stopPreview() {
	o2::jam::fadeOutAndStop(&preview, 0.3f);
}
```

## Fades add no delay

A fade doesn't delay anything: [play](../reference/play.md) starts the sound immediately, at volume 0, and the volume rises from there. BASS changes the volume as it mixes, so the fade is smooth and nothing has to be called every frame. [fadeVolume](../reference/fadeVolume.md) and [fadeOutAndStop](../reference/fadeOutAndStop.md) return immediately.

## Switching songs quickly

When the player scrolls through songs quickly, a fading-out preview would be cut off by loading the next song into the same Stream. For a crossfade, use two Streams and alternate between them: fade one out while the other loads and fades in.

```cpp
o2::jam::Stream previews[2] = {};
int current = 0;

void switchPreview(const char* path, double previewTime) {
	o2::jam::fadeOutAndStop(&previews[current], 0.3f);
	current = 1 - current;
	startPreview(&previews[current], path, previewTime);
}
```

## Looping the preview

[setLooping](../reference/setLooping.md) loops back to the start of the song, not the preview point. To loop a preview, check [isPlaying](../reference/isPlaying.md) on UI updates and start it again from the preview point when it has ended.
