#pragma once

struct GameSettings
{
	enum DIFFICULTY { EASY, MEDIUM, HARD, EXTREME, TRANSCENDENT}; // possible difficulties
	DIFFICULTY difficulty_;
	bool music_; // toggles music on and off
	bool sound_effects_; // toggles sound effects on and off
};