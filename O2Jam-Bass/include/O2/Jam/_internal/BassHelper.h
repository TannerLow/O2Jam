#pragma once
#include <bass/bass.h>
#include <stdio.h>
#include <string>

namespace o2 {
	namespace jam {
		namespace helper {

			inline void audioError(const std::string& text) {
				printf("[Error] (%d): %s\n", BASS_ErrorGetCode(), text.c_str());
				BASS_Free();
				exit(0);
			}

			inline void audioWarning(const std::string& text) {
				printf("[Warning] (%d): %s\n", BASS_ErrorGetCode(), text.c_str());
			}

		} // namespace helper
	} // namespace jam
} // namespace o2
