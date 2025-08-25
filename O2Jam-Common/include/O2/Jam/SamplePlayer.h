#pragma once
#include <memory>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

namespace o2 {
	namespace jam {

		class SamplePlayer {
		protected:
			// Deferring data members to backend implementation
			struct Impl;
			std::unique_ptr<Impl> impl;

		public:
			SamplePlayer();
			~SamplePlayer();

			bool load(const fs::path& filepath, uint8_t maxSimultaneousPlaybacks = 1);
			bool unload();
			bool play();
			bool setVolume(float volume);
		};

	} // namespace jam
} // namespace o2