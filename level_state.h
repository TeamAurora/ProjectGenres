#pragma once
#include "appstate.h"

#include "Contact_listener.h"

#include "Button.h"
#include "living_object.h"
#include "game_object.h"
#include "Player.h"
#include "enemy.h"
#include "pickup.h"
#include "blade.h"
#include "bullet.h"

#include "CollisionTile.h"

#include <vector>
#include <array>

class LevelState : public AppState
{
public:

	virtual ~LevelState();

	void InitializeState();
	void TerminateState();
	APPSTATE Update(const float& ticks_, const int& frame_counter_, const abfw::SonyControllerInputManager& controller_manager_);
	void Render(const float frame_rate_, abfw::Font& font_, abfw::SpriteRenderer* sprite_renderer_);

private:

	virtual void LoadAssets() = 0;	// force loading assets
	virtual void UpdateGameObjects(const float& ticks_, const int& frame_counter_) = 0; // force an update loop function
	virtual APPSTATE InputLoop(const abfw::SonyController* controller) = 0; // force an inputloop that returns a state for next frame
	virtual void CreateObjects() = 0; // force creating the level
	virtual void Restart() = 0; // force a restart level function

	APPSTATE PauseInputLoop(const abfw::SonyController* controller);

	APPSTATE current_state_;

	// Background layers
	struct LevelMap
	{
		std::vector<abfw::Texture*> textures;
		std::vector<Sprite> high_layer;
		std::vector<Sprite> mid_layer;
		std::vector<Sprite> low_layer;
		std::vector<CollisionTile*> collision_layer;
	};
	LevelMap level_map_;

	Sprite pause_background_;
	abfw::Texture* pause_background_texture_;
	std::array<Button*, 3> pause_buttons_;
	enum PAUSEBUTTONSELECTION { RESUME, RESTART, QUIT };
	PAUSEBUTTONSELECTION pause_selection_;
	bool paused_;

protected:

	LevelState(abfw::Platform& platform, const GameApplication* application, abfw::AudioManager* audio_manager, APPSTATE state);

	void LoadMap(const char* map_filename);

	Contact_Listener contact_listener_;
	b2World* world_;

	// Spawn functions
	void SpawnSpike(b2Vec2 spawn_position, b2Vec2 dimensions);
	void SpawnPickup(b2Vec2 spawn_position,  b2Vec2 dimensions, PickUp::PICKUPTYPE type);

	void Destroy(GameObject &object); //will destroy non-destroyed body thats been passed in

	// Pickup Textures
	abfw::Texture* red_pickup_texture_;
	abfw::Texture* blue_pickup_texture_;
	abfw::Texture* yellow_pickup_texture_;
	abfw::Texture* green_pickup_texture_;

	// Plant Textures
	abfw::Texture* plant_wall_texture_;
	abfw::Texture* plant_block_texture_;

	// Spike Texture
	abfw::Texture* spike_texture_;

	// Player Textures
	abfw::Texture* playerArrow;
	abfw::Texture* playerTex;
	abfw::Texture* rotPlayerTex;
	abfw::Texture* playerIdle;
	abfw::Texture* rotPlayerIdle;
	abfw::Texture* playerDeath;
	abfw::Texture* rotPlayerDeath;
	abfw::Texture* playerJump;
	abfw::Texture* rotPlayerJump;
	abfw::Texture* playerAttack;
	abfw::Texture* rotPlayerAttack;
	abfw::Texture* playerFlying;

	// Enemy Textures
	abfw::Texture* enemyMove;
	abfw::Texture* enemyDeath;
	abfw::Texture* enemyAttack;
	abfw::Texture* enemyHit;
	abfw::Texture* enemyIdle;
	abfw::Texture* shooterProjectile;

	// STATE-SPECIFIC Game/Living Objects
	std::vector<PickUp> pickups_;
	std::vector<Enemy> enemies_;
	std::vector<Bullet> bullets_;
	std::vector<LivingObject> plants_;
	std::vector<GameObject> spikes_;
	Player player_;
	Blade blade_;
	Sprite arrow_; //arrow showing where player wants to jump

	// STATE-SPECIFIC Variables
	int score_;		// Score for this level
	int max_score_;
	int collectables_;
	int max_collectable_count_;
	float time_;
	bool gameOver_;		// Track current level status
	float attackTime;	// Amount of time between when attack can be pressed
	float reloadTime;	// Time between shots from the enemy
};