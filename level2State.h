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
#define PLATFORM_NUM2 10
#define PICKUP_NUM2 8
#define PLANT_NUM2 5
#define SPIKE_NUM2 1

class Level2State :
	public AppState
{

public:
	Level2State(abfw::Platform& platform, const GameApplication* application, abfw::AudioManager* audio_manager);
	virtual ~Level2State();
	
	void InitializeState();
	void TerminateState();
	GAMESTATE Update(const float& ticks_, const int& frame_counter_, const abfw::SonyControllerInputManager& controller_manager_);
	void Render(const float frame_rate_, abfw::Font& font_, abfw::SpriteRenderer* sprite_renderer_);
	
private: // PRIVATE METHODS (FUNCTIONS)

	void LoadTextures();
	void LoadSounds();
	void InputLoop(const abfw::SonyController* controller);
	void UpdateGameObjects(const float& ticks_, const int& frame_counter_); // updates game object - deleting flagged ones, updating positions and spawning new ones

	
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
	
	// STATE-SPECIFIC Sound effects
	
	// STATE-SPECIFIC Game/Living Objects
////John////////////////////
	//TODO : Arrays to replaced with vectors
	/*std::vector<GameObject> platforms_;
	std::vector<GameObject> pickups_;
	std::vector<LivingObject> plants_;
	std::vector<GameObject> spikes_;*/
	Blade blade2_;
	Bullet bullet2_;
	Enemy enemy2_;
	PickUp pickUp2_[PICKUP_NUM2];
	GameObject plant2_[PLANT_NUM2];
	GameObject platforms2_[PLATFORM_NUM2];
	Player player2_;
	GameObject spike2_[SPIKE_NUM2];
	Sprite background_;


	// STATE-SPECIFIC Variables
	float score_;//points per pickups gathered
	bool gameOver_;//if true player has died
	float platformWidth_;
	float attackTime;//amount of time between when attack can be pressed
	float reloadTime;//time between shots from the enemy
////////////////////////////////////
};

