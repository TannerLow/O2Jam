#include <O2/Jam/StreamPlayer.h>
#include <O2/Jam/SamplePlayer.h>
#include <iostream>

int songDemo();
int sampleDemo();

int main() {
	int choice;
	std::cout << "1 for song demo, 2 for sample demo\n > ";
	std::cin >> choice;
	
	if (choice == 1) {
		return songDemo();
	}
	else if (choice == 2) {
		return sampleDemo();
	}
}

int songDemo() {
	o2::jam::StreamPlayer songPlayer;

	if (songPlayer.hasDeviceVisibility) {
		std::vector<o2::jam::DeviceInfo> devices = songPlayer.getDevices();
		std::cout << "Audio devices:" << std::endl;
		for (auto deviceInfo : devices) {
			std::cout << deviceInfo.id << ", " << deviceInfo.name << std::endl;
		}
		std::cout << std::endl;
	}

	std::cout << "Loaded song: " << songPlayer.load(L"resources/example_song.mp3") << std::endl;
	// careful, volume 1.0 is very loud
	std::cout << "Adjusting volume: " << songPlayer.setVolume(0.05f) << std::endl;
	std::cout << "Starting song: " << songPlayer.play() << std::endl;

	system("pause");

	std::cout << "Paused song: " << songPlayer.pause() << std::endl;

	system("pause");

	std::cout << "Starting song: " << songPlayer.play() << std::endl;

	system("pause");

	std::cout << "Seeking song: " << songPlayer.seekTo(60.33) << std::endl;

	system("pause");

	std::cout << "Speeding up: " << songPlayer.applyRate(2.0f, false) << std::endl;

	system("pause");

	std::cout << "Swapping device: " << songPlayer.setDevice(4) << std::endl;
	system("pause");
	std::cout << "Swapping device: " << songPlayer.setDevice(2) << std::endl;

	system("pause");

	std::cout << "Slowing down: " << songPlayer.applyRate(1.0f, false) << std::endl;

	system("pause");

	std::cout << "Song length: " << songPlayer.getLength() << std::endl;
	std::cout << "Disabling looping: " << songPlayer.setLooping(false) << std::endl;
	std::cout << "Seeking song: " << songPlayer.seekTo(310) << std::endl;

	system("pause");

	std::cout << "Stopping song: " << songPlayer.stop() << std::endl;
	std::cout << "Playing song: " << songPlayer.play() << std::endl;

	system("pause");

	return 0;
}

#include <thread>
#include <chrono>

int sampleDemo() {
	o2::jam::SamplePlayer samplePlayer;

	// Should use songs with the sample player but im doing so here to show multiple simultaneous playbacks
	std::cout << "Loaded sample: " << samplePlayer.load(L"resources/example_song.mp3", 2) << std::endl;
	// careful, volume 1.0 is very loud
	std::cout << "Adjusting volume: " << samplePlayer.setVolume(0.05f) << std::endl;

	for (int i = 0; i < 3; i++) {
		std::cout << "Starting sample: " << samplePlayer.play() << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}

	system("pause");

	return 0;
}
