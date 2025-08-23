#pragma once
#include <memory>
#include <string>

namespace o2 {
	namespace jam {

		class SamplePlayer {
		protected:
			// Deferring data members to backend implementation
			struct Impl;
			std::unique_ptr<Impl> impl;

		public:
			SamplePlayer() {}
			~SamplePlayer();

			bool load(const std::wstring& filepath, uint8_t maxChannels=1);
			bool play();
			bool setVolume(float volume);
			bool free();
		};

	} // namespace jam
} // namespace o2