#pragma once
#include <bass/bass.h>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <sstream>

namespace o2 {
	namespace jam {
		namespace helper {

			inline std::unordered_map<DWORD, int> deviceUseCount;

			inline void audioError(const std::string& text) {
				printf("[Error] (%d): %s\n", BASS_ErrorGetCode(), text.c_str());
				BASS_Free();
				exit(0);
			}

			inline void audioWarning(const std::string& text) {
				printf("[Warning] (%d): %s\n", BASS_ErrorGetCode(), text.c_str());
			}

			inline void tryFreeDevice() {
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

		} // namespace helper
	} // namespace jam
} // namespace o2
