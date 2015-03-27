#pragma once
#include "appstate.h"

#include "Contact_listener.h"
#include "NLTmxMap.h"

#include "Button.h"
#include "game_object.h"
#include "living_object.h"
#include "Player.h"
#include "enemy.h"
#include "collectible.h"
#include "blade.h"
#include "bullet.h"

#include <vector>
#include <array>

class LevelState : public AppState
{
public:
	LevelState(abfw::Platform& platform, const GameApplication* application, abfw::AudioManager* audio_manager, APPSTATE state);
	virtual ~LevelState();

	void InitializeState();
	void TerminateState();
	APPSTATE Update(const float& ticks_, const int& frame_counter_, const abfw::SonyControllerInputManager& controller_manager_);
	void Render(const float frame_rate_, abfw::Font& font_, abfw::SpriteRenderer* sprite_renderer_);

private:

	void LoadAssets();

	void Pause(bool);

	// override these in derived level classes
	virtual void LoadTextures() = 0;	// force textures
	virtual void LoadSounds();			// do not force sound
	virtual void UpdateGameObjects(const float& ticks_, const int& frame_counter_);
	virtual APPSTATE InputLoop(const abfw::SonyController* controller) = 0; // force an inputloop that returns a state for next frame

	///John///
	void CreateObjects();//call create functions for all gameobjects
	void PlantPickUps();//spawn pickups from plants when they are destroyed

	void Restart();//resets and respawns everything to it's orginal position
	void Destroy(GameObject &object);//will destroy non-destroyed body thats been passed in
	/////////

private:

	APPSTATE current_state_;

	b2World* world_;
	Contact_Listener contact_listener_;

	// Background layers
	struct LevelMap
	{
		std::vector<Sprite> high_layer;
		std::vector<Sprite> mid_layer;
		std::vector<Sprite> low_layer;
		std::vector<b2Body*> collision_layer;
	};
	LevelMap level_map_;

	Sprite pause_background_;
	std::array<Button, 3> pause_buttons_;
	bool paused_;

protected:

	void LoadMap(string map_filename);

	// SHARED Textures
	// Store single-object textures in the object it's used for (e.g. player textures)
	abfw::Texture* red_pickup_texture_;
	abfw::Texture* blue_pickup_texture_;
	abfw::Texture* yellow_pickup_texture_;
	abfw::Texture* green_pickup_texture_;
	abfw::Texture* plantWallTex;
	abfw::Texture* plantBlockTex;
	//abfw::Texture* spikeTexture;
	
	// STATE-SPECIFIC Game/Living Objects
	//std::vector<GameObject> platforms_;
	std::array<GameObject, 20> pickups_;
	//std::vector<LivingObject> plants_;
	//std::vector<GameObject> spikes_;
	Player player_;
	Blade blade_;
	Bullet bullet_;
	Enemy enemy_;
	Sprite arrow_; //arrow showing where player wants to jump

	// STATE-SPECIFIC Variables
	float score_;		// Score for this level
	bool gameOver_;		// Track current level status
	float attackTime;	// Amount of time between when attack can be pressed
	float reloadTime;	//time between shots from the enemy
};