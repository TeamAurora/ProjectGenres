#pragma once
#include <audio/audio_manager.h>
#include <abertay_framework.h>

namespace abfw
{
	class AudioManagerFmod :
		public AudioManager
	{
	public:
		AudioManagerFmod();
		virtual ~AudioManagerFmod();
	};

}