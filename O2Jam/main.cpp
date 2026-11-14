// O2Jam demo: a console program that plays a song and a hitsound and lets
// you try everything with the keyboard. Run it from the O2Jam project folder
// (Visual Studio's default working directory) so resources/ is found, with
// bass.dll and bass_fx.dll next to the executable.

#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <o2/jam/audio.h>
#include <o2/jam/stream.h>
#include <o2/jam/sample.h>

static const char* SONG_PATH = "resources/example_song.mp3";
static const char* HIT_PATH = "resources/example_sample.wav";
static const int MAX_DEVICES = 16;

static void printHelp() {
    printf(
        "\n"
        "  space  play / pause          s  stop (back to start)\n"
        "  ,  .   seek -5 s / +5 s       l  looping on / off\n"
        "  1      normal speed           2  double time (1.5x, same pitch)\n"
        "  3      nightcore (1.5x, higher pitch)\n"
        "  4      half time (0.75x, same pitch)\n"
        "  +  -   song volume            [  ]  all streams volume\n"
        "  h j k  hitsound: center / left / right\n"
        "  p      preview: from 0:30, fade in over 1 s\n"
        "  o      fade out over 1 s and stop\n"
        "  d      choose output device\n"
        "  ?      this help              q  quit\n"
        "\n"
    );
}

static const char* rateName(const o2::jam::Stream* song) {
    if (song->rate == 1.0f) {
        return "normal";
    }
    if (song->rateMode == o2::jam::RateMode::KeepPitch) {
        return song->rate > 1.0f ? "double time" : "half time";
    }
    return song->rate > 1.0f ? "nightcore" : "daycore";
}

static void printStatus(const o2::jam::Stream* song) {
    double position = o2::jam::getPosition(song);
    printf(
        "\r%s  %3d:%02d / %d:%02d  %-11s  volume %.2f  streams %.2f  %s     ",
        o2::jam::isPlaying(song) ? "playing" : "stopped",
        (int)position / 60, (int)position % 60,
        (int)song->length / 60, (int)song->length % 60,
        rateName(song),
        o2::jam::getVolume(song),
        o2::jam::getStreamsVolume(),
        song->looping ? "looping" : ""
    );
}

static void report(
    bool ok,
    const char* what
) {
    if (!ok) {
        printf("\n%s failed: %s\n", what, o2::jam::getErrorMessage());
    }
}

static void chooseDevice() {
    o2::jam::Device devices[MAX_DEVICES];
    int count = o2::jam::listDevices(devices, MAX_DEVICES);
    if (count > MAX_DEVICES) {
        count = MAX_DEVICES;
    }

    printf("\n\nOutput devices (current: %d):\n", o2::jam::getDevice());
    for (int i = 0; i < count && i < 10; i++) {
        printf(
            "  %d  %s%s%s\n",
            i,
            devices[i].name,
            devices[i].isDefault ? " [default]" : "",
            devices[i].isEnabled ? "" : " [unplugged]"
        );
    }
    printf("Press a number, or any other key to cancel\n");

    int key = _getch();
    int choice = key - '0';
    if (choice < 0 || choice >= count || choice >= 10) {
        return;
    }
    if (o2::jam::setDevice(devices[choice].index)) {
        printf("Now on %s, latency %.0f ms\n", devices[choice].name, o2::jam::getLatency() * 1000.0f);
    } else {
        report(false, "Switching device");
    }
}

static float clampVolume(float volume) {
    if (volume < 0.0f) {
        return 0.0f;
    }
    if (volume > 1.0f) {
        return 1.0f;
    }
    return volume;
}

int main() {
    SetConsoleOutputCP(CP_UTF8); // device names are UTF-8

    if (!o2::jam::init(o2::jam::DEFAULT_DEVICE)) {
        printf("Can't start audio: %s\n", o2::jam::getErrorMessage());
        return 1;
    }
    printf("Audio started, latency %.0f ms\n", o2::jam::getLatency() * 1000.0f);

    // Start quiet: 1 is each file's own level, which is loud for a song
    o2::jam::setStreamsVolume(0.25f);
    o2::jam::setSamplesVolume(0.5f);

    o2::jam::Stream song = {};
    o2::jam::Sample hit = {};
    if (!o2::jam::load(&song, SONG_PATH)) {
        printf("Can't load %s: %s\n", SONG_PATH, o2::jam::getErrorMessage());
        o2::jam::shutdown();
        return 1;
    }
    if (!o2::jam::load(&hit, HIT_PATH, 8)) {
        printf("Can't load %s: %s\n", HIT_PATH, o2::jam::getErrorMessage());
    }
    printf("Loaded %s: %.1f s at %.0f Hz\n", SONG_PATH, song.length, song.baseFrequency);

    printHelp();

    bool running = true;
    while (running) {
        while (_kbhit()) {
            int key = _getch();
            if (key == 0 || key == 224) {
                _getch(); // arrow and function keys come as two codes; ignore them
                continue;
            }

            switch (key) {
            case ' ':
                if (o2::jam::isPlaying(&song)) {
                    report(o2::jam::pause(&song), "Pause");
                } else {
                    // fadeOutAndStop leaves the volume at 0
                    if (o2::jam::getVolume(&song) == 0.0f) {
                        o2::jam::setVolume(&song, 1.0f);
                    }
                    report(o2::jam::play(&song), "Play");
                }
                break;
            case 's': report(o2::jam::stop(&song), "Stop"); break;
            case ',': {
                double position = o2::jam::getPosition(&song) - 5.0;
                report(o2::jam::seek(&song, position < 0.0 ? 0.0 : position), "Seek");
                break;
            }
            case '.': report(o2::jam::seek(&song, o2::jam::getPosition(&song) + 5.0), "Seek"); break;
            case 'l': report(o2::jam::setLooping(&song, !song.looping), "Looping"); break;
            case '1': report(o2::jam::setRate(&song, 1.0f, o2::jam::RateMode::KeepPitch), "Rate"); break;
            case '2': report(o2::jam::setRate(&song, 1.5f, o2::jam::RateMode::KeepPitch), "Rate"); break;
            case '3': report(o2::jam::setRate(&song, 1.5f, o2::jam::RateMode::ChangePitch), "Rate"); break;
            case '4': report(o2::jam::setRate(&song, 0.75f, o2::jam::RateMode::KeepPitch), "Rate"); break;
            case '+':
            case '=': report(o2::jam::setVolume(&song, o2::jam::getVolume(&song) + 0.1f), "Volume"); break;
            case '-': report(o2::jam::setVolume(&song, o2::jam::getVolume(&song) - 0.1f), "Volume"); break;
            case ']': o2::jam::setStreamsVolume(clampVolume(o2::jam::getStreamsVolume() + 0.05f)); break;
            case '[': o2::jam::setStreamsVolume(clampVolume(o2::jam::getStreamsVolume() - 0.05f)); break;
            case 'h': report(o2::jam::play(&hit), "Hitsound"); break;
            case 'j': report(o2::jam::play(&hit, hit.volume, -0.8f), "Hitsound"); break;
            case 'k': report(o2::jam::play(&hit, hit.volume, 0.8f), "Hitsound"); break;
            case 'p':
                report(o2::jam::seek(&song, 30.0), "Seek");
                o2::jam::setVolume(&song, 0.0f);
                report(o2::jam::play(&song), "Play");
                report(o2::jam::fadeVolume(&song, 1.0f, 1.0f), "Fade");
                break;
            case 'o': report(o2::jam::fadeOutAndStop(&song, 1.0f), "Fade out"); break;
            case 'd': chooseDevice(); break;
            case '?': printHelp(); break;
            case 'q': running = false; break;
            }
        }

        printStatus(&song);
        Sleep(15);
    }

    printf("\n");
    o2::jam::unload(&hit);
    o2::jam::unload(&song);
    o2::jam::shutdown();
    return 0;
}
