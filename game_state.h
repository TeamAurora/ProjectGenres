#pragma once
#include "appstate.h"
#include "game_object.h"
#include "living_object.h" // lowest in sprite hierarchy - also includes gameobject/animatedsprite/abfw::sprite and Box2D
#include "Player.h"
#include "Contact_listener.h"
#include <vector>
#include "enemy.h"
#include "collectible.h"
#include "blade.h"
#include "bullet.h"


//total number of each objects used for array size
#define PLATFORM_NUM 8
#define PICKUP_NUM 8
#define PLANT_NUM 5
#define SPIKE_NUM 2

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

	// Spawn functions
	void SpawnSpike(b2Vec3 position, b2Vec2 dimensions); // Takes a position (3D to include depth the sprite gets rendered at) and it's dimensions (GFX scale, not Box2D)
///John///
	void CreateObjects();//call create functions for all gameobjects
	void PlantPickUps();//spawn pickups from plants when they are destroyed

	void Restart();//resets and respawns everything to it's orginal position
	void Destroy(GameObject &object);//will destroy non-destroyed body thats been passed in
/////////
private: // PRIVATE MEMBERS (VARIABLES/OBJECTS)
	
	// Box2D world pointer
	b2World* world_;
	Contact_Listener contact_listener_;
	
	// SHARED Textures
	// Store single-object textures in the game object it's used for
	// player textures
	abfw::Texture* playerTex;
	abfw::Texture* rotPlayerTex;	
	abfw::Texture* playerIdle;
	abfw::Texture* playerDeath;
	// pickup textures
	abfw::Texture* redPUTex;
	abfw::Texture* bluePUTex;
	abfw::Texture* platformTex;
	// plant textures
	abfw::Texture* plantWallTex;
	abfw::Texture* plantBlockTex;
	abfw::Texture* rotPlantBlockTex;
	// spike texture - TODO get spike texture
	abfw::Texture* spikeTexture;
	//arrow texture for jump aim
	abfw::Texture* arrowTex;

	
	// STATE-SPECIFIC Sound effects
	
	// STATE-SPECIFIC Game/Living Objects
////John////////////////////
	//TODO : Arrays to replaced with vectors
	/*std::vector<GameObject> platforms_;
	std::vector<GameObject> pickups_;
	std::vector<LivingObject> plants_;
	std::vector<GameObject> spikes_;*/
	Blade blade_;
	Bullet bullet_;
	Enemy enemy_;
	PickUp pickUp_[PICKUP_NUM];
	GameObject plant_[PLANT_NUM];
	GameObject platforms_[PLATFORM_NUM];
	Player player_;
	GameObject spike_[SPIKE_NUM];
	Sprite background_;
	Sprite arrow_;//arrow showing where player wants to jump

	// STATE-SPECIFIC Variables
	float score_;//points per pickups gathered
	bool gameOver_;//if true player has died
	float platformWidth_;
	float attackTime;//amount of time between when attack can be pressed
	float reloadTime;//time between shots from the enemy
////////////////////////////////////
};

