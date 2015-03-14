#pragma once

struct GameSettings
{
	GameSettings() :
		music_(true),
		sound_effects_(true)
		{
		}

	bool music_; // toggles music on and off
	bool sound_effects_; // toggles sound effects on and off
};