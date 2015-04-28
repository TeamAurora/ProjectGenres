#pragma once
//Craig
struct GameSettings
{
	GameSettings(bool music = true, bool sound_effects_ = true) :
		music_(music),
		sound_effects_(sound_effects_)
		{
		}

	bool music_; // toggles music on and off
	bool sound_effects_; // toggles sound effects on and off
};