#include <O2/Jam/StreamPlayer.h>
#include <iostream>

int main() {
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