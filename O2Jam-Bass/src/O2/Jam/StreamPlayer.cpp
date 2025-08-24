#include <O2/Jam/StreamPlayer.h>
#include <unordered_map>
#include <bass/bass.h>
#include <bass/bass_fx.h>
#include <O2/Jam/_internal/BassHelper.h>
#include <sstream>

namespace o2 {
	namespace jam {

		static std::unordered_map<DWORD, int> deviceUseCount;

		static void tryFreeDevice() {
			bool shouldFree = true;
			DWORD device = BASS_GetDevice();
			if (device != (DWORD)(-1)) {
				deviceUseCount[device]--;
				if (deviceUseCount[device] > 0) {
					// Dont free device if its still in use
					shouldFree = false;
				}
			}

			if (shouldFree and !BASS_Free()) {
				std::stringstream ss;
				ss << "Failed to free device (" << ") while destructing StreamPlayer";

				int errorCode = BASS_ErrorGetCode();
				if (errorCode == BASS_ERROR_INIT) {
					ss << ", Reason: device not initialized";
					helper::audioWarning(ss.str());
				}
				else {
					ss << ", Reason: device currently being reinitialized";
					helper::audioWarning(ss.str());
				}
			}
		}

		struct StreamPlayer::Impl {
			HSTREAM stream;
			fs::path currentFile;
			float volume = -1;
			int baseFrequency;
			double lengthSeconds = -1;
			bool playing = false;
			bool loopEnabled = false;
			float rate = 1.f;
			bool pitchAdjusted = true;
		};

		StreamPlayer::StreamPlayer() : hasDeviceVisibility(true) {
			impl = std::make_unique<Impl>();

			// If there isnt a device currently initialized then initialize one
			std::optional<DeviceInfo> currentDevice = getCurrentDevice();
			if (!currentDevice.has_value()) {
				if (!BASS_Init(-1, 48000, 0, 0, nullptr)) {
					helper::audioError("Failed to initialize the default audio device");
				}
				DWORD deviceId = BASS_GetDevice();
				if (deviceId == (DWORD)(-1)) {
					helper::audioError("Failed to get ID of newly initialized audio device");
				}
				else {
					deviceUseCount[deviceId]++;
				}
			}
		}

		StreamPlayer::~StreamPlayer() {
			tryFreeDevice();
		}

		bool StreamPlayer::load(const fs::path& filepath) {
			if (impl->stream) {
				if (impl->currentFile == filepath) {
					return true;
				}
				else {
					if (!unload()) {
						helper::audioError("Failed to unload audio stream.");
						return false;
					}
				}
			}

			impl->currentFile = filepath;

			// not using sourceStream to see if I dont need it
			impl->stream = BASS_StreamCreateFile(
				FALSE, filepath.u16string().c_str(),
				0,
				0,
				BASS_UNICODE | BASS_SAMPLE_FLOAT | BASS_STREAM_DECODE
			);
			if (!impl->stream) {
				helper::audioError("Failed to create audio stream from file.");
				return false;
			}

			BASS_CHANNELINFO channelInfo;
			if (!BASS_ChannelGetInfo(impl->stream, &channelInfo)) {
				helper::audioError("Failed to get frequency of channel.");
				return false;
			}
			impl->baseFrequency = channelInfo.freq;

			impl->stream = BASS_FX_TempoCreate(impl->stream, BASS_FX_FREESOURCE);
			if (!impl->stream) {
				helper::audioError("Failed to create tempo stream from audio stream.");
				return false;
			}

			if (!BASS_ChannelSetAttribute(impl->stream, BASS_ATTRIB_TEMPO_OPTION_USE_QUICKALGO, 1)) {
				helper::audioError("Error occurred while enabling quick audio tempo algorithm.");
				return false;
			}
			// Quaver uses and claims osu also uses these settings but chatGPT says USE_QUICKALGO disables these
			// Should consider enabling if audio lag is a problem
			//BASS_ChannelSetAttribute(handle, BASS_ATTRIB_TEMPO_OPTION_OVERLAP_MS, 4);
			//BASS_ChannelSetAttribute(handle, BASS_ATTRIB_TEMPO_OPTION_SEQUENCE_MS, 30);

			QWORD lengthBytes = BASS_ChannelGetLength(impl->stream, BASS_POS_BYTE);
			if (lengthBytes == (QWORD)(-1)) {
				helper::audioError("Error occured in retrieving channel length.");
				return false;
			}
			impl->lengthSeconds = BASS_ChannelBytes2Seconds(impl->stream, lengthBytes);
			if (impl->lengthSeconds < 0) {
				helper::audioError("Error occured in conversion from bytes to seconds.");
				return false;
			}

			if (impl->volume >= 0) {
				setVolume(impl->volume);
			}

			if (!setLooping(impl->loopEnabled)) {
				helper::audioWarning("Failed to update the looping attribute when loading audio file");
			}

			if (!applyRate(impl->rate, impl->pitchAdjusted)) {
				helper::audioWarning("Failed to update the rate when loading audio file");
			}

			return true;
		}

		bool StreamPlayer::unload() {
			if (impl->stream) {
				if (isPlaying() and !stop()) {
					helper::audioWarning("[Error] Failed to stop audio stream.\n");
				}
				if (!BASS_StreamFree(impl->stream)) {
					helper::audioError("[Error] Failed to free audio stream.\n");
					return false;
				}
				impl->stream = 0;
			}

			return true;
		}

		bool StreamPlayer::play() {
			if (!impl->stream) {
				return false;
			}

			// Documentation seems to be wrong. Starting a paused channel works fine
			bool success = BASS_ChannelPlay(impl->stream, FALSE);
			if (!success) {
				helper::audioError("Error occured when attempting to play audio stream.");
			}
			else {
				impl->playing = true;
			}

			return success;
		}

		bool StreamPlayer::stop() {
			if (!impl->stream) {
				return false;
			}

			bool success = BASS_ChannelStop(impl->stream);
			if (!success) {
				helper::audioError("Error occured when attempting to stop audio stream.");
			}
			else {
				impl->playing = false;
			}

			return success;
		}

		bool StreamPlayer::pause() {
			if (!impl->stream or !impl->playing) {
				return false;
			}

			impl->playing = false;

			bool success = BASS_ChannelPause(impl->stream);
			if (!success) {
				helper::audioWarning("Error occued while attempting to pause audio channel");
			}

			return success;
		}

		bool StreamPlayer::setVolume(float volume) {
			impl->volume = volume;

			if (!impl->stream) {
				helper::audioWarning("Attempted to set volume of an unloaded audio stream.");
				return false;
			}

			bool success = BASS_ChannelSetAttribute(impl->stream, BASS_ATTRIB_VOL, volume);
			if (!success) {
				helper::audioWarning("Error occured when setting volume of audio channel.");
			}

			return success;
		}

		bool StreamPlayer::applyRate(float rate, bool adjustPitch) {
			bool success = false;
			if (adjustPitch) {
				success = BASS_ChannelSetAttribute(impl->stream, BASS_ATTRIB_TEMPO_FREQ, impl->baseFrequency);
				success &= BASS_ChannelSetAttribute(impl->stream, BASS_ATTRIB_TEMPO, rate * 100 - 100);
			}
			else {
				success = BASS_ChannelSetAttribute(impl->stream, BASS_ATTRIB_TEMPO, 0);
				success &= BASS_ChannelSetAttribute(impl->stream, BASS_ATTRIB_TEMPO_FREQ, impl->baseFrequency * rate);
			}

			if (!success) {
				helper::audioError("Error occured while changing rate of audio.");
			}
			else {
				impl->rate = rate;
				impl->pitchAdjusted = adjustPitch;
			}

			return success;
		}

		float StreamPlayer::getRate() const {
			return impl->rate;
		}

		bool StreamPlayer::seekTo(double timeSeconds) {
			if (timeSeconds >= impl->lengthSeconds) {
				helper::audioWarning("Attempted to seek audio stream past end");
				return false;
			}

			QWORD positionBytes = BASS_ChannelSeconds2Bytes(impl->stream, timeSeconds);
			if (positionBytes == (QWORD)(-1)) {
				return false;
			}

			bool success = BASS_ChannelSetPosition(impl->stream, positionBytes, BASS_POS_BYTE);

			return success;
		}

		bool StreamPlayer::setLooping(bool shouldLoop) {
			if (!(impl->lengthSeconds > 0 and impl->stream)) {
				return false;
			}

			DWORD currentFlags = BASS_ChannelFlags(impl->stream, 0, 0);
			if (currentFlags == (DWORD)(-1)) {
				helper::audioWarning("Failed to retrieve attribute flags for audio channel");
				return false;
			}

			if (currentFlags & BASS_SAMPLE_LOOP) { // looping is currently enabled
				if (!shouldLoop) {
					return BASS_ChannelFlags(impl->stream, 0, BASS_SAMPLE_LOOP) != (DWORD)(-1); // remove the LOOP flag
				}
			}
			else { // looping is currently disabled
				if (shouldLoop) {
					return BASS_ChannelFlags(impl->stream, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP) != (DWORD)(-1); // set the LOOP flag
				}
			}

			return true; // looping setting was already set to the desired setting
		}

		bool StreamPlayer::isPlaying() const {
			return impl->playing;
		}

		double StreamPlayer::getLength() const {
			if (!impl->stream) {
				return -1;
			}

			return impl->lengthSeconds;
		}

		double StreamPlayer::getPosition() const {
			QWORD positionInBytes = BASS_ChannelGetPosition(impl->stream, BASS_POS_BYTE);
			if (positionInBytes < 0) {
				helper::audioWarning("Failed to get byte position of audio channel");
				return false;
			}
			return BASS_ChannelBytes2Seconds(impl->stream, positionInBytes);
		}

		// Backend dependent, check hasMultiDeviceSupport
		std::vector<DeviceInfo> StreamPlayer::getDevices() const {
			std::vector<DeviceInfo> deviceInfos;

			BASS_DEVICEINFO info;
			for (DWORD device = 1; BASS_GetDeviceInfo(device, &info); device++) {
				DeviceInfo deviceInfo;
				deviceInfo.id = device;
				deviceInfo.name = std::string(info.name); // TODO update with utf support
				deviceInfo.enabled = info.flags & BASS_DEVICE_ENABLED;

				deviceInfos.push_back(deviceInfo);
			}

			return deviceInfos;
		}

		// Backend dependent, check hasMultiDeviceSupport
		std::optional<DeviceInfo> StreamPlayer::getCurrentDevice() const {
			long bassCurrentDevice = (long)BASS_GetDevice();

			if (bassCurrentDevice >= 0) {
				BASS_DEVICEINFO bassDeviceInfo;
				if (BASS_GetDeviceInfo((DWORD)bassCurrentDevice, &bassDeviceInfo)) {
					DeviceInfo deviceInfo;
					deviceInfo.id = bassCurrentDevice;
					deviceInfo.name = std::string(bassDeviceInfo.name);
					deviceInfo.enabled = bassDeviceInfo.flags & BASS_DEVICE_ENABLED;
				}
			}

			return std::nullopt;
		}

		// Backend dependent, check hasMultiDeviceSupport
		bool StreamPlayer::setDevice(long deviceId) {
			// If there is a stream loaded to the current device, transfer it to the new device
			if (impl->stream) {
				double position = getPosition();
			
				std::optional<DeviceInfo> currentDevice = getCurrentDevice();
				if (!currentDevice.has_value()) {
					tryFreeDevice();
				}
			
				if (!BASS_Init(deviceId, 48000, 0, 0, nullptr)) {
					std::stringstream ss;
					ss << "Failed to initialize audio device #" << deviceId;
					helper::audioError(ss.str());
					return false;
				}
			
				impl->stream = 0;
			
				if (!load(impl->currentFile)) {
					helper::audioError("Failed to load stream onto new device");
					return false;
				}
			
				if (!seekTo(position)) {
					helper::audioWarning("Failed to seek stream on new device");
				}
			
				if (isPlaying()) {
					if (!play()) {
						helper::audioError("Failed to play audio stream");
						return false;
					}
				}
			}
			else {
				std::optional<DeviceInfo> currentDevice = getCurrentDevice();
				if (!currentDevice.has_value()) {
					tryFreeDevice();
				}

				if (!BASS_Init(deviceId, 48000, 0, 0, nullptr)) {
					std::stringstream ss;
					ss << "Failed to initialize audio device #" << deviceId;
					helper::audioError(ss.str());
					return false;
				}
			}

			return true;
		}

	} // namespace jam
} // namespace o2