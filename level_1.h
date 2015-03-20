#pragma once
#include "level_state.h"

class Level_1 :
	public LevelState
{

public:
	Level_1(abfw::Platform& platform, const GameApplication* application, abfw::AudioManager* audio_manager);
	virtual ~Level_1();
	
private: // PRIVATE METHODS (FUNCTIONS)

	void LoadTextures();
	void LoadSounds();
	void LoadMap();
	APPSTATE InputLoop(const abfw::SonyController* controller);
	void UpdateGameObjects(const float& ticks_, const int& frame_counter_); // updates game object - deleting flagged ones, updating positions and spawning new ones

	// Spawn functions
	void SpawnSpike(b2Vec3 position, b2Vec2 dimensions); // Takes a position (3D to include depth the sprite gets rendered at) and it's dimensions (GFX scale, not Box2D)

};

