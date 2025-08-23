#pragma once
#include <memory>
#include <string>

namespace o2 {
	namespace jam {

		class StreamPlayer {
		protected:
			// Deferring data members to backend implementation
			struct Impl;
			std::unique_ptr<Impl> impl;

		public:
			StreamPlayer();
			virtual ~StreamPlayer();

			bool load(const std::wstring& filepath);
			bool play();
			bool stop();
			bool pause();
			bool setVolume(float volume);
			bool applyRate(float rate, bool adjustPitch);
			bool seekTo(double timeSeconds);
			void setLooping(bool shouldLoop);
			bool isPlaying() const;
			bool free();
		};

	} // namespace jam
} // namespace o2