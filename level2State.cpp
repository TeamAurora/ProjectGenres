#include "level2State.h"
#include <graphics/sprite_renderer.h>
#include <audio/vita/audio_manager_vita.h>
#include <graphics/texture.h>
#include <iostream>
#include "game_application.h"
#include "box2d_helpers.h"

Level2State::Level2State(abfw::Platform& platform, const GameApplication* application, abfw::AudioManager* audio_manager) :
	AppState(platform, application, audio_manager)
{
}


Level2State::~Level2State()
{
}

void Level2State::InitializeState()
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

	player2_.set_texture(playerTex);
	//alternate pickup textures
	for(int g = 0; g < PICKUP_NUM2;g+=2)
	{
		pickUp2_[g].set_texture(redPUTex);
		pickUp2_[g+1].set_texture(bluePUTex);	
	}

	//for platforms
	for (int q = 0;q < PLATFORM_NUM2; q++)
	{
		platforms2_[q].set_texture(platformTex);
	}

	//for plant walls
	plant2_[0].set_texture(plantWallTex);
	plant2_[1].set_texture(plantWallTex);

	//for plant blocks
	plant2_[2].set_texture(rotPlantBlockTex);
	plant2_[3].set_texture(rotPlantBlockTex);
	plant2_[3].set_uv_width(-1.0f);
	plant2_[4].set_texture(plantBlockTex);
	plant2_[4].set_uv_height(-1.0f);

	// spawn spikes, platforms, etc
	platformWidth_ = GFX_BOX2D_SIZE(10);

	CreateObjects();

	score_ = 0;
	gameOver_ = false;
	reloadTime = 0;
}

void Level2State::TerminateState()
{
	//Destroy(player2_);//player
	//Destroy(enemy2_);//enemy
	//Destroy(bullet2_);

	////destroy pickups
	//for ( int y = 0; y < PICKUP_NUM2; y++)
	//{
	//	//reset for another play
	//	pickUp2_[y].dead = true;
	//	pickUp2_[y].spawned = false;

	//	Destroy(pickUp2_[y]);
	//}

	//for(int s = 0; s < PLANT_NUM2; s++)
	//{
	//	//destroy plants
	//	Destroy(plant2_[s]);
	//}

	//for(int d = 0; d < SPIKE_NUM2; d++)
	//{
	//	//destroy plants
	//	Destroy(spike2_[d]);
	//}

	//for(int h = 0; h < PLATFORM_NUM2; h++)
	//{
	//	//destroy plants
	//	Destroy(platforms2_[h]);
	//}

	// Cleanup resources and pointers
	delete world_;
	world_ = NULL;
}

GAMESTATE Level2State::Update(const float& ticks_, const int& frame_counter_, const abfw::SonyControllerInputManager& controller_manager_)
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
		if (controller->buttons_down() & ABFW_SONY_CTRL_START)
		{
			return INTRO; // go to intro if select is down. TODO change state
		}
		else // do input loop for state if we aren't returning to menustate
			InputLoop(controller);
	}

	return LEVEL2; // else continue with gamestate
}

void Level2State::Render(const float frame_rate_, abfw::Font& font_, abfw::SpriteRenderer* sprite_renderer_)
{
	// Draw game objects
	sprite_renderer_->DrawSprite(background_);

	if(player2_.dead == false)
	{
		sprite_renderer_->DrawSprite(player2_);
		if(blade2_.destroyed == false)
		{
			sprite_renderer_->DrawSprite(blade2_);
		}
	}
	
	if(enemy2_.dead == false)
	{
		sprite_renderer_->DrawSprite(enemy2_);
	}

	if(bullet2_.dead == false)
	{
		sprite_renderer_->DrawSprite(bullet2_);
	}

	for(int u = 0; u < PICKUP_NUM2; u++)
	{
		if(pickUp2_[u].dead == false)
		{
			sprite_renderer_->DrawSprite(pickUp2_[u]);
		}
	}

	for(int g = 0; g < PLANT_NUM2; g++)
	{
		if(plant2_[g].dead == false)
		{
			sprite_renderer_->DrawSprite(plant2_[g]);
		}
	}

	for(int a = 0; a < SPIKE_NUM2; a++)
	{
		sprite_renderer_->DrawSprite(spike2_[a]);
	}

	//draw platfroms
	for (int wall_render = 0;wall_render < PLATFORM_NUM2;wall_render++)
	{
		//platform[wall_render].set_texture(platform_tex);//set texture
		sprite_renderer_->DrawSprite(platforms2_[wall_render]);
	}

	font_.RenderText(sprite_renderer_, abfw::Vector3(10.0f, 5.0f, -0.9f), 1.0f, 0xff00ff00, abfw::TJ_LEFT, "Galaxea");
	font_.RenderText(sprite_renderer_, abfw::Vector3(815.0f, 40.0f, -0.9f), 1.0f, 0xff00ffff, abfw::TJ_LEFT, "health  : %.0f", player2_.health());	//display player health
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

void Level2State::LoadTextures()
{
	//Load textures using application_->LoadTextureFromPNG("texturename.png")
	// single object texture loaded directly into objects
	background_.set_texture(application_->LoadTextureFromPNG("Level_BG.png"));

	// state-level textures 
	playerTex = application_->LoadTextureFromPNG("Robot_animations.png");
	rotPlayerTex = application_->LoadTextureFromPNG("Robot_animations_rotated.png");
	redPUTex = application_->LoadTextureFromPNG("Red.png");
	bluePUTex = application_->LoadTextureFromPNG("Blue.png");
	platformTex = application_->LoadTextureFromPNG("Platform_Panel.png");
	plantWallTex = application_->LoadTextureFromPNG("Plant_Wall.png");
	plantBlockTex = application_->LoadTextureFromPNG("Plant_Block.png");
	rotPlantBlockTex = application_->LoadTextureFromPNG("Plant_Block_rot.png");
}

void Level2State::LoadSounds()
{
	// Load audio using audio_manager_->LoadSample("sfxname.wav", platform_)

}

void Level2State::InputLoop(const abfw::SonyController* controller)
{
	player2_.Player_Input(controller);

	//manually restart
	if (controller->buttons_down() & ABFW_SONY_CTRL_TRIANGLE)
	{
		Restart();
	}

	if (controller->buttons_down() & ABFW_SONY_CTRL_LEFT)
	{
		application_->camera_->MoveBy(abfw::Vector2(-1.0f, 0.0f));
	}

	if (controller->buttons_down() & ABFW_SONY_CTRL_RIGHT)
	{
		application_->camera_->MoveBy(abfw::Vector2(1.0f, 0.0f));
	}

	if (controller->buttons_down() & ABFW_SONY_CTRL_UP)
	{
		application_->camera_->MoveBy(abfw::Vector2(0.0f, -1.0f));
	}

	if (controller->buttons_down() & ABFW_SONY_CTRL_DOWN)
	{
		application_->camera_->MoveBy(abfw::Vector2(0.0f, 1.0f));
	}
}

void Level2State::UpdateGameObjects(const float& ticks_, const int& frame_counter_)
{
	// Any per-frame or per-tick updating for game objects should be done here
	
	//update player if alive
	if(player2_.dead != true)
	{
		player2_.Update(ticks_, gameOver_, true);
	}
	else
	{
		Restart();
	}
	
	//create and update blade
	if(player2_.attacking == true)
	{
		if(blade2_.created == false)
		{
			blade2_.Create(world_, player2_);
			attackTime = 0;
		}
	}
	else if (blade2_.destroyed == false && blade2_.created == true && attackTime > 20)//destroy blade after a certain time
	{
		Destroy(blade2_);
		blade2_.created = false;
	}
	
	if(blade2_.created == true)
	{
		blade2_.Update(ticks_,player2_);
		attackTime += (ticks_*20);
	}

	//update enemy if alive
	if(enemy2_.dead != true)
	{
		enemy2_.Update_Enemy(ticks_, player2_.currentPos, false);
		reloadTime  += (ticks_*20);

		if(reloadTime > 50 && bullet2_.created == false)//shoot a bullet
		{
			bullet2_.CreateBullet(world_,enemy2_.x,enemy2_.y, enemy2_.gravity);
			reloadTime = 0;
		}
	}
	else
	{
		Destroy(enemy2_);//remove enemys
	}

	if(bullet2_.dead != true && bullet2_.created == true)
	{
		bullet2_.Update(ticks_);
	}
	else if (bullet2_.dead == true)
	{
		Destroy(bullet2_);
		bullet2_.created = false;
	}

	//remove from game if collected
	for(int i = 0;i < PICKUP_NUM2;i++)
	{
		if(pickUp2_[i].dead == true && pickUp2_[i].destroyed == false)
		{
			//destroy Pickup
			Destroy(pickUp2_[i]);
			//increase score
			score_+=1;
		}
		else if(pickUp2_[i].dead == false)
		{
			pickUp2_[i].Update(ticks_);
		}
	}

	//change sprite for horizontal or vertical movement
	if (player2_.horizontal == false)
	{
		player2_.set_texture(rotPlayerTex);
	}
	else
	{
		player2_.set_texture(playerTex);
	}

	//check for destroyed plants
	//for(int g = 0; g < PLANT_NUM2;g++)
	//{
	//	if(plant2_[g].dead == true && plant2_[g].destroyed == false)
	//	{
	//		Destroy(plant2_[g]);
	//	}
	//}
	
	//span extra pickup from certain plants
	//PlantPickUps();

	//win condition
	if(score_ == PICKUP_NUM2)
	{
		gameOver_ = true;
	}	
}

void Level2State::CreateObjects()
{
	
	player2_.Create_Player(world_,GFX_BOX2D_POS_X(platform_.width()*0.15),GFX_BOX2D_POS_Y(platform_.height()*0.85));
	enemy2_.Create_Enemy(world_, GFX_BOX2D_POS_X(platform_.width()*0.55f),GFX_BOX2D_POS_Y(platform_.height()*0.1));
	enemy2_.gravity = b2Vec2(0,10);

	//create walls
	//boundries
	platforms2_[0].CreateStaticBody(world_,GFX_BOX2D_POS_X(0),GFX_BOX2D_POS_Y(0),GFX_BOX2D_SIZE(platform_.width()),platformWidth_);//roof
	platforms2_[1].CreateStaticBody(world_,GFX_BOX2D_POS_X(0),GFX_BOX2D_POS_Y(platform_.height()),GFX_BOX2D_SIZE(platform_.width()),platformWidth_);//ground
	platforms2_[2].CreateStaticBody(world_,GFX_BOX2D_POS_X(0),GFX_BOX2D_POS_Y(platform_.height()),platformWidth_,GFX_BOX2D_SIZE(platform_.height()));//left wall
	platforms2_[3].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()),GFX_BOX2D_POS_Y(0),platformWidth_,GFX_BOX2D_SIZE(platform_.height()));//right wall
	//level parts
	platforms2_[4].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.18f),GFX_BOX2D_POS_Y(platform_.height()*0.98f),
	GFX_BOX2D_SIZE(platform_.width()*0.2f),GFX_BOX2D_SIZE(platform_.height()*0.04f));	//bottom left
	platforms2_[5].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.3f),GFX_BOX2D_POS_Y(platform_.height()*0.65f),
		GFX_BOX2D_SIZE(platform_.width()*0.125f),GFX_BOX2D_SIZE(platform_.height()*0.1f));//left outcrop
	platforms2_[6].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.1f),GFX_BOX2D_POS_Y(platform_.height()*0.325f),
		GFX_BOX2D_SIZE(platform_.width()*0.1f),GFX_BOX2D_SIZE(platform_.height()*0.35f));//top left
	platforms2_[7].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.87f),GFX_BOX2D_POS_Y(platform_.height()*0.08f),
		GFX_BOX2D_SIZE(platform_.width()*0.12f),GFX_BOX2D_SIZE(platform_.height()*0.08f));//top right	
	platforms2_[8].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.85f),GFX_BOX2D_POS_Y(platform_.height()*0.8f),
		GFX_BOX2D_SIZE(platform_.width()*0.08f),GFX_BOX2D_SIZE(platform_.height()*0.08f));//right outcrop
	platforms2_[9].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.91f),GFX_BOX2D_POS_Y(platform_.height()*0.7f),
		GFX_BOX2D_SIZE(platform_.width()*0.08f),GFX_BOX2D_SIZE(platform_.height()*0.35f));//right side	

	//set object type
	for(int i = 0;i < PLATFORM_NUM2;i++)
	{
		platforms2_[i].setType(GameObject::PLATFORM);
	}
	
	//create pickups on ceiling
	pickUp2_[0].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.3f),GFX_BOX2D_POS_Y(platform_.height()*0.85f));
	pickUp2_[1].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.5f),GFX_BOX2D_POS_Y(platform_.height()*0.75f));
	pickUp2_[2].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.65f),GFX_BOX2D_POS_Y(platform_.height()*0.1f));
	pickUp2_[3].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.5f),GFX_BOX2D_POS_Y(platform_.height()*0.3f));
	pickUp2_[4].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.25f),GFX_BOX2D_POS_Y(platform_.height()*0.4f));
	pickUp2_[5].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.4f),GFX_BOX2D_POS_Y(platform_.height()*0.1f));
	pickUp2_[6].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.75f),GFX_BOX2D_POS_Y(platform_.height()*0.5f));
	pickUp2_[7].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.95f),GFX_BOX2D_POS_Y(platform_.height()*0.25f));

	//create plants
	//tunnel blockers
	//plant2_[0].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.16f),GFX_BOX2D_POS_Y(platform_.height()*0.82f),GFX_BOX2D_SIZE(20)
	//	,GFX_BOX2D_SIZE(platform_.height()*0.12f));
	//plant2_[1].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.29f),GFX_BOX2D_POS_Y(platform_.height()*0.82),GFX_BOX2D_SIZE(20),
	//	GFX_BOX2D_SIZE(platform_.height()*0.12f));
	////treasure holders
	//plant2_[2].CreateStaticBody(world_,GFX_BOX2D_POS_X(30),GFX_BOX2D_POS_Y(platform_.height()*0.325),GFX_BOX2D_SIZE(20),
	//	GFX_BOX2D_SIZE(20));//left
	//plant2_[3].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.28f),GFX_BOX2D_POS_Y(platform_.height()*0.325),GFX_BOX2D_SIZE(20),
	//	GFX_BOX2D_SIZE(20));//right
	//plant2_[4].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.16f),GFX_BOX2D_POS_Y(30),GFX_BOX2D_SIZE(20)
	//	,GFX_BOX2D_SIZE(20));//top

	//set to plant type
	for(int j = 0; j < PLANT_NUM2;j++)
	{
		plant2_[j].setType(GameObject::PLANT);
	}

	//create spike
	//floor
	spike2_[0].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.6f),GFX_BOX2D_POS_Y(platform_.height()-15),
		GFX_BOX2D_SIZE(platform_.width()*0.25f),GFX_BOX2D_SIZE(5));
	//top left
	//spike2_[1].CreateStaticBody(world_,GFX_BOX2D_POS_X(0),GFX_BOX2D_POS_Y(0),
	//	GFX_BOX2D_SIZE(40),GFX_BOX2D_SIZE(40));
	//spike2_[1].set_rotation(40);
	////top right
	//spike2_[2].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.32f),GFX_BOX2D_POS_Y(5),
	//	GFX_BOX2D_SIZE(40),GFX_BOX2D_SIZE(40));
	//spike2_[2].set_rotation(40);
	////bottom right
	//spike2_[3].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.31f),GFX_BOX2D_POS_Y(platform_.height()*0.545f),
	//	GFX_BOX2D_SIZE(25),GFX_BOX2D_SIZE(25));
	//spike2_[3].set_rotation(40);

	//set to spike type
	for(int k = 0; k < SPIKE_NUM2; k++)
	{
		spike2_[k].set_colour(0xff0000ff);//set colour to red
		spike2_[k].setType(GameObject::SPIKE);
	}
}

void Level2State::Restart()
{
	//destroy objects
	Destroy(player2_);//player
	Destroy(enemy2_);//enemy

	//destroy pickups
	for ( int y = 0; y < PICKUP_NUM2; y++)
	{
		//reset for another play
		pickUp2_[y].dead = true;
		pickUp2_[y].spawned = false;

		Destroy(pickUp2_[y]);
	}

	//for(int s = 0; s < PLANT_NUM2; s++)
	//{
	//	//destroy plants
	//	Destroy(plant2_[s]);
	//}

	score_ = 0;

	//create new versions at starting position
	CreateObjects();

	gameOver_ = false;
}

void Level2State::PlantPickUps()
{
	////left plant
	//if(plant2_[2].destroyed == true && pickUp2_[6].spawned == false)
	//{
	//	pickUp2_[6].Create_pickup(world_,GFX_BOX2D_POS_X(30),GFX_BOX2D_POS_Y(platform_.height()*0.325));
	//	pickUp2_[6].spawned = true;
	//}

	////right plant
	//if(plant2_[3].destroyed == true && pickUp2_[7].spawned == false)
	//{
	//	pickUp2_[7].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.26f),GFX_BOX2D_POS_Y(platform_.height()*0.325));
	//	pickUp2_[7].spawned = true;
	//}

	////top plant
	//if(plant2_[4].destroyed == true && pickUp2_[8].spawned == false)
	//{
	//	pickUp2_[8].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.16f),GFX_BOX2D_POS_Y(30));
	//	pickUp2_[8].spawned = true;
	//}

}

void Level2State::Destroy(GameObject &object)
{
	if(object.destroyed == false)
	{
		world_->DestroyBody(object.body_);
		object.body_ = NULL;
		object.destroyed = true;
	}
}



