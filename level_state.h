#pragma once
#include "appstate.h"

#include "Contact_listener.h"
#include "NLTmxMap.h"

#include "game_object.h"
#include "living_object.h"
#include "Player.h"
#include "enemy.h"
#include "collectible.h"
#include "blade.h"
#include "bullet.h"

#include <vector>

//total number of each objects used for array size
#define PLATFORM_NUM 8
#define PICKUP_NUM 8
#define PLANT_NUM 5
#define SPIKE_NUM 4
#define TILE_TOTAL_COUNT 222

class LevelState : public AppState
{
public:
	LevelState();
	virtual ~LevelState();

	void InitializeState();
	void TerminateState();
	APPSTATE Update(const float& ticks_, const int& frame_counter_, const abfw::SonyControllerInputManager& controller_manager_);
	void Render(const float frame_rate_, abfw::Font& font_, abfw::SpriteRenderer* sprite_renderer_);

private:

	// override these in derived level classes
	virtual void LoadTextures();
	virtual void LoadSounds();
	virtual void ConstructBackgroundVectorsFromMap();
	virtual void ConstructPhysicsBodiesFromMap();
	virtual void RenderGameObjects();
	void RenderBackgroundVectors();

	///John///
	void CreateObjects();//call create functions for all gameobjects
	void PlantPickUps();//spawn pickups from plants when they are destroyed

	void Restart();//resets and respawns everything to it's orginal position
	void Destroy(GameObject &object);//will destroy non-destroyed body thats been passed in
	/////////

private:

	b2World* world_;
	Contact_Listener contact_listener_;
	NLTmxMap* map_;

protected:

	// SHARED Textures
	// Store single-object textures in the game object it's used for
	abfw::Texture* playerTex;
	abfw::Texture* rotPlayerTex;
	abfw::Texture* redPUTex;
	abfw::Texture* bluePUTex;
	abfw::Texture* platformTex;
	abfw::Texture* plantWallTex;
	abfw::Texture* plantBlockTex;
	abfw::Texture* rotPlantBlockTex;
	// spike texture - TODO get spike texture
	//abfw::Texture* spikeTexture;
	

	// STATE-SPECIFIC Game/Living Objects
////John////////////////////
	//TODO : Arrays to replaced with vectors
	/*std::vector<GameObject> platforms_;
	std::vector<GameObject> pickups_;
	std::vector<LivingObject> plants_;
	std::vector<GameObject> spikes_;*/
	Player player_;
	Blade blade_;
	Bullet bullet_;
	Enemy enemy_;
	PickUp pickUp_[PICKUP_NUM];
	GameObject plant_[PLANT_NUM];
	GameObject platforms_[PLATFORM_NUM];
	GameObject spike_[SPIKE_NUM];
	Sprite arrow_; //arrow showing where player wants to jump

	// STATE-SPECIFIC Variables
	float score_;		// Score for this level
	bool gameOver_;		// Track current level status
	float platformWidth_;
	float attackTime;	// Amount of time between when attack can be pressed
	float reloadTime;//time between shots from the enemy
////////////////////////////////////

	// Background Layer Vectors (Low > High Render Order)
	std::vector<Sprite> High_Layer_;
	std::vector<Sprite> Mid_Layer_;
	std::vector<Sprite> Low_Layer_;


	static void* loadFile(const char * filename, bool appendNull);

};