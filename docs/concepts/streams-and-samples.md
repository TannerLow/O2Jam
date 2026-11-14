# Streams and samples

[Index](../index.md) · Previous: [Getting started](getting-started.md) · Next: [Rate and pitch](rate-and-pitch.md)

The library plays audio in two ways, and every sound uses one of them.

| | [Stream](../reference/Stream.md) | [Sample](../reference/Sample.md) |
|---|---|---|
| For | songs, menu music, previews | hitsounds, keysounds, UI clicks |
| Loading | opens the file and reads a little ahead as it plays | decodes the whole file into memory |
| Memory | small, whatever the length | the whole sound, uncompressed: about 11 MB per minute of 48 kHz stereo |
| Playing it again while it plays | does nothing; one playback at a time | starts another playback that overlaps, up to `maxPlaybacks` |
| Pause, seek, position | yes | no; a playback runs from start to end |
| Loop, fade | yes | no |
| [Rate](rate-and-pitch.md) | yes | no: samples always play at normal speed |
| Volume and pan | per stream | per sample, or per playback |

## Formats

Both read MP3, OGG Vorbis, WAV and AIFF files. On Windows they also read the formats Windows has codecs for, such as AAC (`.m4a`) and WMA.

## Streams

A [Stream](../reference/Stream.md) is a struct you own, filled in by [load](../reference/load.md). It plays, [pauses](../reference/pause.md), [stops](../reference/stop.md) (back to the start), [seeks](../reference/seek.md) and [loops](../reference/setLooping.md).

[load](../reference/load.md) scans the whole file once, so seeking lands exactly where asked and the length is exact, even in MP3s that would otherwise only give an estimate. For a long MP3 this makes loading take a little longer, which is fine outside gameplay.

## Samples

A [Sample](../reference/Sample.md) is decoded once when loaded, so playing it is just mixing memory that's already there: nothing is read from disk during gameplay.

`maxPlaybacks`, given to [load](../reference/load.md), is how many times one sample can be heard at once. When all are in use, playing it again cuts off the playback that started longest ago. For a hitsound in a fast song, 4 to 8 is plenty: a hitsound is usually short, so earlier playbacks have ended before they'd be cut. A keysound that's only ever hit by one note at a time needs 1 or 2.

Each sample has a volume and pan that [play](../reference/play.md)`(sample)` uses. The other overload, `play(sample, volume, pan)`, sets them for one playback, for example to pan a hitsound toward the column that was hit.

## Volume

Every volume in the library goes from 0 (silent) to 1, the file's own level: as loud as the file played in a media player at the same Windows volume. Nothing is ever amplified above that, so 1 isn't dangerous, but songs are usually mastered loud, so a game's settings should start lower.

Volumes multiply. The volume heard is the stream's or sample's own volume times the global volume for streams or samples, set with [setStreamsVolume and setSamplesVolume](../reference/setStreamsVolume.md), which fit the music and effects sliders in a game's settings. A stream at 0.75 with the streams volume at 0.25 plays at 0.75 × 0.25 = 0.1875.

These numbers scale the sound wave, and ears don't hear that evenly: 0.5 sounds only a little quieter than 1, and most of a slider's audible change happens near the bottom. Games often square a slider's value before passing it on, so the slider feels even.
