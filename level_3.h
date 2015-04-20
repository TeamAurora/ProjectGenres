#pragma once
#include "level_state.h"

class Level_3 :
	public LevelState
{

public:

	Level_3(abfw::Platform& platform, const GameApplication* application, abfw::AudioManager* audio_manager);
	virtual ~Level_3();
	
private:

	void LoadAssets();
	APPSTATE InputLoop(const abfw::SonyController* controller);
	void UpdateGameObjects(const float& ticks_, const int& frame_counter_); // updates game object - deleting flagged ones, updating positions and spawning new ones
	void CreateObjects();
	void Restart();
};