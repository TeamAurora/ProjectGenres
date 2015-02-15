#include "game_state.h"
#include <graphics/sprite_renderer.h>
#include <audio/vita/audio_manager_vita.h>
#include <graphics/texture.h>
#include <iostream>
#include "game_application.h"
#include "box2d_helpers.h"

GameState::GameState(abfw::Platform& platform, const GameApplication* application, abfw::AudioManager* audio_manager) :
	AppState(platform, application, audio_manager)
{
}


GameState::~GameState()
{
}

void GameState::InitializeState()
{
	//load assetes
	LoadTextures();
	LoadSounds();

	// Box2D Init
	// create new box2d world with zero gravity
	world_ = new b2World(b2Vec2(0.0f, 0.0f));
	world_->SetContactListener(&contact_listener_);

	// Init Objects
/////Set textures////
	background_.set_height(platform_.height());
	background_.set_width(platform_.width());
	background_.set_position(abfw::Vector3(platform_.width()*0.5f, platform_.height()*0.5f, 1.0f));

	player_.set_texture(playerTex);
	//alternate pickup textures
	for(int g = 0; g < PICKUP_NUM;g+=2)
	{
		pickUp_[g].set_texture(redPUTex);
		pickUp_[g+1].set_texture(bluePUTex);	
	}

	//for platforms
	for (int q = 0;q < PLATFORM_NUM; q++)
	{
		platforms_[q].set_texture(platformTex);
	}

	//for plant walls
	plant_[0].set_texture(plantWallTex);
	plant_[1].set_texture(plantWallTex);

	//for plant blocks
	plant_[2].set_texture(rotPlantBlockTex);
	plant_[3].set_texture(rotPlantBlockTex);
	plant_[3].set_uv_width(-1.0f);
	plant_[4].set_texture(plantBlockTex);
	plant_[4].set_uv_height(-1.0f);

	// spawn spikes, platforms, etc
	platformWidth_ = GFX_BOX2D_SIZE(10);

	CreateObjects();

	score_ = 0;
	gameOver_ = false;
}

void GameState::TerminateState()
{
	// Cleanup resources and pointers
	delete world_;
	world_ = NULL;
}

GAMESTATE GameState::Update(const float& ticks_, const int& frame_counter_, const abfw::SonyControllerInputManager& controller_manager_)
{
	// Step box2d world - add if box2d is enabled conditional
	float time_step = 1.0f / 60.0f;
	int32 velocity_iterations = 10;
	int32 position_iterations = 5;
	world_->Step(time_step, velocity_iterations, position_iterations);

	// Do game logic for gameobjects
	UpdateGameObjects(ticks_, frame_counter_);

	// Do input
	const abfw::SonyController* controller = controller_manager_.GetController(0);
	if(controller) // if controller isn't null
	{
		if (controller->buttons_down() & ABFW_SONY_CTRL_SELECT)
		{
			//return MENU; // go to menu if start is down. TODO change state
		}
		else // do input loop for state if we aren't returning to menustate
			InputLoop(controller);
	}

	return GAME; // else continue with gamestate
}

void GameState::Render(const float frame_rate_, abfw::Font& font_, abfw::SpriteRenderer* sprite_renderer_)
{
	// Draw game objects
	sprite_renderer_->DrawSprite(background_);

	if(player_.dead == false)
	{
		sprite_renderer_->DrawSprite(player_);
		if(blade_.destroyed == false)
		{
			sprite_renderer_->DrawSprite(blade_);
		}
	}
	
	if(enemy_.dead == false)
	{
		sprite_renderer_->DrawSprite(enemy_);
	}

	for(int u = 0; u < PICKUP_NUM; u++)
	{
		if(pickUp_[u].dead == false)
		{
			sprite_renderer_->DrawSprite(pickUp_[u]);
		}
	}

	for(int g = 0; g < PLANT_NUM; g++)
	{
		if(plant_[g].dead == false)
		{
			sprite_renderer_->DrawSprite(plant_[g]);
		}
	}

	for(int a = 0; a < SPIKE_NUM; a++)
	{
		sprite_renderer_->DrawSprite(spike_[a]);
	}

	//draw platfroms
	for (int wall_render = 0;wall_render < PLATFORM_NUM;wall_render++)
	{
		//platform[wall_render].set_texture(platform_tex);//set texture
		sprite_renderer_->DrawSprite(platforms_[wall_render]);
	}

	//text temporaily broken. Not loading in comic_sans font
	font_.RenderText(sprite_renderer_, abfw::Vector3(10.0f, 5.0f, -0.9f), 1.0f, 0xff00ff00, abfw::TJ_LEFT, "Project Zero");
	font_.RenderText(sprite_renderer_, abfw::Vector3(815.0f, 40.0f, -0.9f), 1.0f, 0xff00ffff, abfw::TJ_LEFT, "health  : %.0f", player_.health());	//display player health
	font_.RenderText(sprite_renderer_, abfw::Vector3(815.0f, 5.0f, -0.9f), 1.0f, 0xff00ffff, abfw::TJ_LEFT, "Score : %.0f", score_);//display player score
	font_.RenderText(sprite_renderer_, abfw::Vector3(850.0f, 510.0f, -0.9f), 1.0f, 0xff00ffff, abfw::TJ_LEFT, "FPS: %.1f", frame_rate_);

	if(gameOver_ == true)
	{
		font_.RenderText(sprite_renderer_, abfw::Vector3(350.0f,200.0f,0.9f), 3.0f, 0xff00ffff, abfw::TJ_LEFT, "YOU WIN!!");
		font_.RenderText(sprite_renderer_, abfw::Vector3(350.0f,250.0f,0.9f), 3.0f, 0xff00ffff, abfw::TJ_LEFT, "Press Triangle");
		font_.RenderText(sprite_renderer_, abfw::Vector3(350.0f,300.0f,0.9f), 3.0f, 0xff00ffff, abfw::TJ_LEFT, "to replay");
	}

	// tell sprite renderer that all sprites have been drawn
	sprite_renderer_->End();
}

void GameState::LoadTextures()
{
	//Load textures using application_->LoadTextureFromPNG("texturename.png")

	// single object texture loaded directly into objects
	background_.set_texture(application_->LoadTextureFromPNG("Level_BG.png"));

	// state-level textures 
	playerTex = application_->LoadTextureFromPNG("Character.png");
	rotPlayerTex = application_->LoadTextureFromPNG("Character_rotate.png");
	redPUTex = application_->LoadTextureFromPNG("Red.png");
	bluePUTex = application_->LoadTextureFromPNG("Blue.png");
	platformTex = application_->LoadTextureFromPNG("Platform_Panel.png");
	plantWallTex = application_->LoadTextureFromPNG("Plant_Wall.png");
	plantBlockTex = application_->LoadTextureFromPNG("Plant_Block.png");
	rotPlantBlockTex = application_->LoadTextureFromPNG("Plant_Block_rot.png");
}

void GameState::LoadSounds()
{
	// Load audio using audio_manager_->LoadSample("sfxname.wav", platform_)

}

void GameState::InputLoop(const abfw::SonyController* controller)
{
	player_.Player_Input(controller);

	//manually restart
	if (controller->buttons_down() & ABFW_SONY_CTRL_TRIANGLE)
	{
		Restart();
	}
}

void GameState::UpdateGameObjects(const float& ticks_, const int& frame_counter_)
{
	// Any per-frame or per-tick updating for game objects should be done here
	
	//update player if alive
	if(player_.dead != true)
	{
		player_.Update(ticks_, gameOver_);
	}
	else
	{
		Restart();
	}
	
	//create and update blade
	if(player_.attacking == true)
	{
		if(blade_.created == false)
		{
			blade_.Create(world_, player_);
			attackTime = 0;
		}
	}
	else if (blade_.destroyed == false && blade_.created == true && attackTime > 20)
	{
		//destroy blade
		Destroy(blade_);
		blade_.created = false;
	}
	
	if(blade_.created == true)
	{
		blade_.Update(ticks_,player_);
		attackTime ++;
	}

	//update enemy if alive
	if(enemy_.dead != true)
	{
		enemy_.Update_Enemy(ticks_, player_.currentPos);
	}
	else
	{
		Destroy(enemy_);//remove enemys
	}

	//remove from game if collected
	for(int i = 0;i < PICKUP_NUM;i++)
	{
		if(pickUp_[i].dead == true && pickUp_[i].destroyed == false)
		{
			//destroy Pickup
			Destroy(pickUp_[i]);
			//increase score
			score_+=1;
		}
		else if(pickUp_[i].dead == false)
		{
			pickUp_[i].Update(ticks_);
		}
	}

	//change sprite for horizontal or vertical movement
	if (player_.horizontal == false)
	{
		player_.set_texture(rotPlayerTex);
	}
	else
	{
		player_.set_texture(playerTex);
	}

	//check for destroyed plants
	for(int g = 0; g < PLANT_NUM;g++)
	{
		if(plant_[g].dead == true && plant_[g].destroyed == false)
		{
			Destroy(plant_[g]);
		}
	}
	
	//span extra pickup from certain plants
	PlantPickUps();

	//win condition
	if(score_ == PICKUP_NUM)
	{
		gameOver_ = true;
	}	
}

void GameState::CreateObjects()
{
	player_.Create_Player(world_, platform_.width(), platform_.height());
	enemy_.Create_Enemy(world_, GFX_BOX2D_POS_X(platform_.width()*0.35f),GFX_BOX2D_POS_Y(platform_.height()*0.3));
	enemy_.gravity = b2Vec2(-10,0);

	//create walls
	//boundries
	platforms_[0].CreateStaticBody(world_,GFX_BOX2D_POS_X(0),GFX_BOX2D_POS_Y(0),GFX_BOX2D_SIZE(platform_.width()),platformWidth_);//roof
	platforms_[1].CreateStaticBody(world_,GFX_BOX2D_POS_X(0),GFX_BOX2D_POS_Y(platform_.height()),GFX_BOX2D_SIZE(platform_.width()),platformWidth_);//ground
	platforms_[2].CreateStaticBody(world_,GFX_BOX2D_POS_X(0),GFX_BOX2D_POS_Y(platform_.height()),platformWidth_,GFX_BOX2D_SIZE(platform_.height()));//left wall
	platforms_[3].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()),GFX_BOX2D_POS_Y(0),platformWidth_,GFX_BOX2D_SIZE(platform_.height()));//right wall
	//level parts
	platforms_[4].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.9f),GFX_BOX2D_POS_Y(platform_.height()*0.9f),
		GFX_BOX2D_SIZE(platform_.width()*0.125f),GFX_BOX2D_SIZE(platform_.height()*0.125f));	//bottom right
	platforms_[5].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.175f),GFX_BOX2D_POS_Y(platform_.height()*0.95f),
		GFX_BOX2D_SIZE(platform_.width()*0.25f),GFX_BOX2D_SIZE(platform_.height()*0.08f));//botttom left
	platforms_[6].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.325f),GFX_BOX2D_POS_Y(platform_.height()*0.36f),
		GFX_BOX2D_SIZE(platform_.width()*0.025f),GFX_BOX2D_SIZE(platform_.height()*0.35f));//divider
	platforms_[7].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.22f),GFX_BOX2D_POS_Y(platform_.height()*0.63f),
		GFX_BOX2D_SIZE(platform_.width()*0.1f),GFX_BOX2D_SIZE(platform_.height()*0.08f));//tunnel roof

	//set object type
	for(int i = 0;i < PLATFORM_NUM;i++)
	{
		platforms_[i].setType(GameObject::PLATFORM);
	}
	
	//create pickups on ceiling
	pickUp_[0].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.75f),GFX_BOX2D_POS_Y(30));
	pickUp_[1].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.7f),GFX_BOX2D_POS_Y(30));
	pickUp_[2].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.65f),GFX_BOX2D_POS_Y(30));
	pickUp_[3].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.6f),GFX_BOX2D_POS_Y(30));
	pickUp_[4].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.55f),GFX_BOX2D_POS_Y(30));
	pickUp_[5].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.5f),GFX_BOX2D_POS_Y(30));
	pickUp_[9].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.8f),GFX_BOX2D_POS_Y(30));

	//create plants
	//tunnel blockers
	plant_[0].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.16f),GFX_BOX2D_POS_Y(platform_.height()*0.82f),GFX_BOX2D_SIZE(20)
		,GFX_BOX2D_SIZE(platform_.height()*0.12f));
	plant_[1].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.29f),GFX_BOX2D_POS_Y(platform_.height()*0.82),GFX_BOX2D_SIZE(20),
		GFX_BOX2D_SIZE(platform_.height()*0.12f));
	//treasure holders
	plant_[2].CreateStaticBody(world_,GFX_BOX2D_POS_X(30),GFX_BOX2D_POS_Y(platform_.height()*0.325),GFX_BOX2D_SIZE(20),
		GFX_BOX2D_SIZE(20));//left
	plant_[3].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.28f),GFX_BOX2D_POS_Y(platform_.height()*0.325),GFX_BOX2D_SIZE(20),
		GFX_BOX2D_SIZE(20));//right
	plant_[4].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.16f),GFX_BOX2D_POS_Y(30),GFX_BOX2D_SIZE(20)
		,GFX_BOX2D_SIZE(20));//top

	//set to plant type
	for(int j = 0; j < PLANT_NUM;j++)
	{
		plant_[j].setType(GameObject::PLANT);
	}

	//create spike
	//floor
	spike_[0].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.575f),GFX_BOX2D_POS_Y(platform_.height()-15),
		GFX_BOX2D_SIZE(platform_.width()*0.2f),GFX_BOX2D_SIZE(5));
	//top left
	spike_[1].CreateStaticBody(world_,GFX_BOX2D_POS_X(0),GFX_BOX2D_POS_Y(0),
		GFX_BOX2D_SIZE(40),GFX_BOX2D_SIZE(40));
	spike_[1].set_rotation(40);
	//top right
	spike_[2].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.32f),GFX_BOX2D_POS_Y(5),
		GFX_BOX2D_SIZE(40),GFX_BOX2D_SIZE(40));
	spike_[2].set_rotation(40);
	//bottom right
	spike_[3].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.31f),GFX_BOX2D_POS_Y(platform_.height()*0.545f),
		GFX_BOX2D_SIZE(25),GFX_BOX2D_SIZE(25));
	spike_[3].set_rotation(40);

	//set to spike type
	for(int k = 0; k < SPIKE_NUM; k++)
	{
		spike_[k].set_colour(0xff0000ff);//set colour to red
		spike_[k].setType(GameObject::SPIKE);
	}
}

void GameState::Restart()
{
	//destroy objects
	Destroy(player_);//player
	Destroy(enemy_);//enemy

	//destroy pickups
	for ( int y = 0; y < PICKUP_NUM; y++)
	{
		//reset for another play
		pickUp_[y].dead = true;
		pickUp_[y].spawned = false;

		Destroy(pickUp_[y]);
	}

	for(int s = 0; s < PLANT_NUM; s++)
	{
		//destroy plants
		Destroy(plant_[s]);
	}

	score_ = 0;

	//create new versions at starting position
	CreateObjects();

	gameOver_ = false;
}

void GameState::PlantPickUps()
{
	//left plant
	if(plant_[2].destroyed == true && pickUp_[6].spawned == false)
	{
		pickUp_[6].Create_pickup(world_,GFX_BOX2D_POS_X(30),GFX_BOX2D_POS_Y(platform_.height()*0.325));
		pickUp_[6].spawned = true;
	}

	//right plant
	if(plant_[3].destroyed == true && pickUp_[7].spawned == false)
	{
		pickUp_[7].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.26f),GFX_BOX2D_POS_Y(platform_.height()*0.325));
		pickUp_[7].spawned = true;
	}

	//top plant
	if(plant_[4].destroyed == true && pickUp_[8].spawned == false)
	{
		pickUp_[8].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.16f),GFX_BOX2D_POS_Y(30));
		pickUp_[8].spawned = true;
	}

}

void GameState::Destroy(GameObject &object)
{
	if(object.destroyed == false)
	{
		world_->DestroyBody(object.body_);
		object.body_ = NULL;
		object.destroyed = true;
	}
}

//
//void GameState::SpawnSpike(b2Vec3 position_, b2Vec2 dimensions_)
//{
//	float x_pos_ = position_.x;
//	float y_pos_ = position_.y;
//	float width_ = dimensions_.x;
//	float height_ = dimensions_.y;
//
//	GameObject spike_ = GameObject();
//
//	// Define and add box2d body
//	b2BodyDef body_;
//	body_.type = b2_staticBody;
//	body_.position = b2Vec2(GFX_BOX2D_POS_X(x_pos_), GFX_BOX2D_POS_Y(y_pos_));
//	spike_.AddBody(world_, body_); // this also changes this object to use box2d physics in all its functions
//
//	// Define and add box2d fixture
//	b2FixtureDef fixture_;
//	b2PolygonShape shape_;
//	shape_.SetAsBox(GFX_BOX2D_SIZE(dimensions_.x), GFX_BOX2D_SIZE(dimensions_.y));
//	fixture_.shape = &shape_;
//	fixture_.density = 0.0f;
//	spike_.AddFixture(fixture_);
//
//	// Initialize all the gameobject related things for this spike
//	spike_.InitSprite(dimensions_.x, dimensions_.y, abfw::Vector3(x_pos_, y_pos_, 0.0f), spike_texture); // init sprite properties
//	spike_.set_type(GameObject::SPIKE);
//	spike_.set_visibility(true);			// Makes projectile visible
//
//	// adds this spike to the spikes vector
//	spikes_.push_back(spike_);
//
//}