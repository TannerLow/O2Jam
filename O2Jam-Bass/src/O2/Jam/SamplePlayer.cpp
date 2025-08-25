#include <O2/Jam/SamplePlayer.h>
#include <O2/Jam/_internal/BassHelper.h>

namespace o2 {
	namespace jam {

		struct SamplePlayer::Impl {
			HSAMPLE sample = 0;
			fs::path currentFile;
			float volume = 0;
			// bool loopingEnabled = false;
		};

		SamplePlayer::SamplePlayer() {
			impl = std::make_unique<Impl>();

			// If there isnt a device currently initialized then initialize one
			DWORD currentDeviceId = BASS_GetDevice();
			if (currentDeviceId == (DWORD)(-1)) {
				if (!BASS_Init(-1, 48000, 0, 0, nullptr)) {
					helper::audioError("Failed to initialize the default audio device");
				}

				DWORD deviceId = BASS_GetDevice();
				if (deviceId == (DWORD)(-1)) {
					helper::audioError("Failed to get ID of newly initialized audio device");
				}
				else {
					helper::deviceUseCount[deviceId]++;
				}
			}
		}

		SamplePlayer::~SamplePlayer() {
			// free the sample since device may remain if still in use by others
			if (!unload()) {
				helper::audioError("Failed to unload audio sample");
			}
			helper::tryFreeDevice();
		}

		bool SamplePlayer::load(const fs::path& filepath, uint8_t maxSimultaneousPlaybacks) {
			if (impl->sample) {
				if (impl->currentFile == filepath) {
					helper::audioWarning("Attempted to load already loaded sample. Ignoring maxSimultaneousPlaybacks");
					return true;
				}
				else {
					if (!unload()) {
						helper::audioError("Failed to unload audio sample.");
						return false;
					}
				}
			}

			impl->currentFile = filepath;

			// not using sourceStream to see if I dont need it
			impl->sample = BASS_SampleLoad(
				FALSE, 
				filepath.u16string().c_str(), 
				0, 
				0, 
				maxSimultaneousPlaybacks, 
				BASS_UNICODE
			);
			if (!impl->sample) {
				helper::audioError("Failed to create audio sample from file.");
				return false;
			}

			if (impl->volume >= 0) {
				setVolume(impl->volume);
			}

			// TODO add looping support to samples, maybe
			//if (!setLooping(impl->loopEnabled)) {
			//	helper::audioWarning("Failed to update the looping attribute when loading audio file");
			//}

			return true;
		}

		bool SamplePlayer::unload() {
			if (impl->sample) {
				if (!BASS_SampleFree(impl->sample)) {
					helper::audioError("Failed to free audio sample.");
					return false;
				}
				impl->sample = 0;
			}

			return true;
		}

		bool SamplePlayer::play() {
			unsigned long channel = BASS_SampleGetChannel(impl->sample, BASS_SAMPLE_OVER_POS);
			if (!channel) {
				helper::audioWarning("Failed to get audio sample channel");
				return false;
			}

			if (!BASS_ChannelPlay(channel, true)) {
				helper::audioWarning("Failed to start audio sample channel");
				return false;
			}

			return true;
		}

		bool SamplePlayer::setVolume(float volume) {
			impl->volume = volume;

			BASS_SAMPLE info;
			if (!BASS_SampleGetInfo(impl->sample, &info)) {
				helper::audioError("Failed to get sample info to update volume.");
				return false;
			}
			info.volume = volume;
			if (!BASS_SampleSetInfo(impl->sample, &info)) {
				helper::audioError("Failed to set sample volume.");
				return false;
			}

			return true;
		}

	} // namespace jam
} // namespace o2