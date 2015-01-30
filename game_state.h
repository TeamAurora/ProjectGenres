#pragma once
#include "appstate.h"
#include "game_object.h"
#include "living_object.h" // lowest in sprite hierarchy - also includes gameobject/animatedsprite/abfw::sprite and Box2D
#include "Player.h"
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
	
private: // PRIVATE METHODS (FUNCTIONS)

	void LoadTextures();
	void LoadSounds();
	void InputLoop(const abfw::SonyController* controller);
	void UpdateGameObjects(const float& ticks_, const int& frame_counter_); // updates game object - deleting flagged ones, updating positions and spawning new ones
	void CollisionDetectionLoop();

private: // PRIVATE STRUCTS

	struct Blade
	{
		float 
	};

private: // PRIVATE MEMBERS (VARIABLES/OBJECTS)
	
	// Box2D world pointer
	b2World* world_;
	
	// SHARED Textures
	// Store single textures in a game object
	abfw::Texture* red_pickup_;
	abfw::Texture* blue_pickup_;
	abfw::Texture* platform_texture_;
	abfw::Texture* plant_block_texture_;
	abfw::Texture* plant_wall_texture_;
	abfw::Texture* background_;

	
	// STATE-SPECIFIC Sound effects
	
	// STATE-SPECIFIC Game/Living Objects
	std::vector<GameObject> platforms_;
	std::vector<GameObject> pickups_;
	std::vector<LivingObject> plants_;
	std::vector<GameObject> spikes_;
	Player player_;

	// STATE-SPECIFIC Variables
	float score_;
};

