#pragma once
#include "appstate.h"
#include "living_object.h" // lowest in sprite hierarchy - also includes gameobject/animatedsprite/abfw::sprite and Box2D
#include <vector>

class GameState :
	public AppState
{
public:
	GameState(abfw::Platform& platform, const GameApplication* application, abfw::AudioManager* audio_manager);
	virtual ~GameState();
	
	void InitializeState();
	void TerminateState();
	GAMESTATE Update(const float& ticks_, const int& frame_counter_, const abfw::SonyControllerInputManager& controller_manager_);
	void Render(const float frame_rate_, abfw::Font& font_, abfw::SpriteRenderer* sprite_renderer_);
	
private:

	// Function Prototypes
	void LoadTextures();
	void LoadSounds();
	void InputLoop(const abfw::SonyController* controller);
	void UpdateGameObjects(const float& ticks_, const int& frame_counter_); // updates game object - deleting flagged ones, updating positions and spawning new ones
	void CollisionDetectionLoop();
	
	// Box2D world pointer
	b2World* world_;
	
	// Textures
	
	// Sound effects
	
	// Game Objects

	// Variables
};

