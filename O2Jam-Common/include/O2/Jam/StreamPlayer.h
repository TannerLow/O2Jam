#pragma once
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <filesystem>

namespace fs = std::filesystem;

namespace o2 {
	namespace jam {
		struct DeviceInfo {
			long id;
			std::string name; // create utf library since this should always be utf-8
			bool enabled;
		};

		class StreamPlayer {
		protected:
			// Deferring data members to backend implementation
			struct Impl;
			std::unique_ptr<Impl> impl;

		public:
			const bool hasDeviceVisibility;

		public:
			StreamPlayer();
			virtual ~StreamPlayer();

			bool load(const fs::path& filepath);
			bool unload();
			bool play();
			bool stop();
			bool pause();
			bool setVolume(float volume);
			bool applyRate(float rate, bool adjustPitch);
			float getRate() const;
			bool seekTo(double timeSeconds);
			bool setLooping(bool shouldLoop);
			bool isPlaying() const;
			double getLength() const;
			double getPosition() const;

			// Backend dependent, check hasDeviceVisibility
			std::vector<DeviceInfo> getDevices() const;
			
			// Backend dependent, check hasDeviceVisibility
			std::optional<DeviceInfo> getCurrentDevice() const;
			
			// Backend dependent, check hasDeviceVisibility
			bool setDevice(long deviceId);
		};

	} // namespace jam
} // namespace o2