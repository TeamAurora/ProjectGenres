#include "level_2.h"
#include <graphics/sprite_renderer.h>
#include <audio/vita/audio_manager_vita.h>
#include <graphics/texture.h>
#include <iostream>
#include "game_application.h"
#include "box2d_helpers.h"

Level_2::Level_2(abfw::Platform& platform, const GameApplication* application, abfw::AudioManager* audio_manager) :
	AppState(platform, application, audio_manager, LEVEL_2)
{
}

Level_2::~Level_2()
{
}

void Level_2::LoadAssets()
{
	//Load textures using application_->LoadTextureFromPNG("texturename.png")
	// single object texture loaded directly into objects
	//background_.set_texture(application_->LoadTextureFromPNG("Level_BG.png"));

	//// state-level textures 
	////player
	//playerTex = application_->LoadTextureFromPNG("Robot_animations.png");
	//rotPlayerTex = application_->LoadTextureFromPNG("Robot_animations_rotated.png");
	//playerIdle = application_->LoadTextureFromPNG("Robot_Animation_Idle.png");	
	//rotPlayerIdle = application_->LoadTextureFromPNG("Robot_Animation_Idle_rot.png");
	//playerDeath = application_->LoadTextureFromPNG("Robot_Animation_death.png");
	//rotPlayerDeath = application_->LoadTextureFromPNG("Robot_Animation_death_rot.png");
	//playerJump = application_->LoadTextureFromPNG("Robot_AnimatioN_JUMP.png");
	//rotPlayerJump = application_->LoadTextureFromPNG("Robot_AnimatioN_JUMP_rot.png");

	////enemy
	//enemyShooting = application_->LoadTextureFromPNG("shooting.png");
	//shooterDeath = application_->LoadTextureFromPNG("shooter_death.png");

	//redPUTex = application_->LoadTextureFromPNG("Red.png");
	//bluePUTex = application_->LoadTextureFromPNG("Blue.png");
	//platformTex = application_->LoadTextureFromPNG("Platform_Panel.png");
	//plantWallTex = application_->LoadTextureFromPNG("Plant_Wall.png");
	//plantBlockTex = application_->LoadTextureFromPNG("Plant_Block.png");
	//rotPlantBlockTex = application_->LoadTextureFromPNG("Plant_Block_rot.png");
}

APPSTATE Level_2::InputLoop(const abfw::SonyController* controller)
{
	//so player can't move when dead
	if(player_.dead != true)
	{
		player_.Player_Input(controller);
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

void Level_2::UpdateGameObjects(const float& ticks_, const int& frame_counter_)
{
	// Any per-frame or per-tick updating for game objects should be done here
	
	//update player if alive
	if(player_.dead != true)
	{
		player_.Update(ticks_, gameOver_, true);
	}
	else if (player_.deadAnim == false)
	{
		player_.Update(ticks_, gameOver_,false);
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
	else if (blade_.destroyed == false && blade_.created == true && attackTime > 20)//destroy blade after a certain time
	{
		Destroy(blade_);
		blade_.created = false;
	}
	
	if(blade_.created == true)
	{
		blade_.Update(ticks_, player_);
		attackTime += (ticks_*20);
	}

	//update enemy if alive
	if(enemy2_.dead != true)
	{
		enemy2_.Update_Enemy(ticks_, player2_.currentPos, false);
		reloadTime  += (ticks_*20);

		if(reloadTime > 50 && bullet2_.created == false)//shoot a bullet at intervals
		{
			enemy2_.shooting = true;

			if(enemy2_.shotFired)//shoot animation done
			{
				//bullet2_.CreateBullet(world_,enemy2_.x,enemy2_.y, enemy2_.gravity,  player2_.currentPos);//fire
				//reset 
				reloadTime = 0;
				enemy2_.shotFired = false;
			}
		}
		else
		{
			enemy2_.shooting = false;
		}
	}
	else if(enemy2_.deadAnim == false)//play death animation
	{
		enemy2_.Update_Enemy(ticks_, player2_.currentPos, false);
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
		//change textures
		switch(player2_.state_)
		{
			case Player::IDLE:
					player2_.set_texture(rotPlayerIdle);
					break;
			case Player::RUNNING:
					player2_.set_texture(rotPlayerTex);
					break;			 
			case Player::ATTACKING:

				break;
			case Player::DEAD:
				player2_.set_texture(rotPlayerDeath);
				break;
			case Player::JUMPING:
				player2_.set_texture(rotPlayerJump);
				break;
		};
	}
	else
	{
		//change textures
		switch(player2_.state_)
		{
			case Player::IDLE:
					player2_.set_texture(playerIdle);
					break;
			case Player::RUNNING:
					player2_.set_texture(playerTex);
					break;			 
			case Player::ATTACKING:

				break;
			case Player::DEAD:
				player2_.set_texture(playerDeath);
				break;
			case Player::JUMPING:
				player2_.set_texture(playerJump);
				break;
		};
	}

	switch(enemy2_.shooterState_)
	{
		case Enemy::IDLE:
			enemy2_.set_texture(enemyShooting);
			break;
		case Enemy::SHOOTING:
			enemy2_.set_texture(enemyShooting);
			break;
		case Enemy::DEAD:			
			enemy2_.set_texture(shooterDeath);
			break;
	};

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

//void Level_2::CreateObjects()
//{
//	
//	player2_.Create_Player(world_,GFX_BOX2D_POS_X(platform_.width()*0.15),GFX_BOX2D_POS_Y(platform_.height()*0.85));
//	enemy2_.Create_Enemy(world_, GFX_BOX2D_POS_X(platform_.width()*0.55f),GFX_BOX2D_POS_Y(platform_.height()*0.1));
//	enemy2_.gravity = b2Vec2(0,10);
//
//	//create walls
//	//boundries
//	if(platforms2_[0].dead == true)//check if platforms has been created only needs to check one as they are all done at the same time
//	{
//		platforms2_[0].CreateStaticBody(world_,GFX_BOX2D_POS_X(0),GFX_BOX2D_POS_Y(0),GFX_BOX2D_SIZE(platform_.width()),platformWidth_);//roof
//		platforms2_[1].CreateStaticBody(world_,GFX_BOX2D_POS_X(0),GFX_BOX2D_POS_Y(platform_.height()),GFX_BOX2D_SIZE(platform_.width()),platformWidth_);//ground
//		platforms2_[2].CreateStaticBody(world_,GFX_BOX2D_POS_X(0),GFX_BOX2D_POS_Y(platform_.height()),platformWidth_,GFX_BOX2D_SIZE(platform_.height()));//left wall
//		platforms2_[3].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()),GFX_BOX2D_POS_Y(0),platformWidth_,GFX_BOX2D_SIZE(platform_.height()));//right wall
//		//level parts
//		platforms2_[4].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.18f),GFX_BOX2D_POS_Y(platform_.height()*0.98f),
//		GFX_BOX2D_SIZE(platform_.width()*0.2f),GFX_BOX2D_SIZE(platform_.height()*0.04f));	//bottom left
//		platforms2_[5].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.3f),GFX_BOX2D_POS_Y(platform_.height()*0.65f),
//			GFX_BOX2D_SIZE(platform_.width()*0.125f),GFX_BOX2D_SIZE(platform_.height()*0.1f));//left outcrop
//		platforms2_[6].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.1f),GFX_BOX2D_POS_Y(platform_.height()*0.325f),
//			GFX_BOX2D_SIZE(platform_.width()*0.1f),GFX_BOX2D_SIZE(platform_.height()*0.35f));//top left
//		platforms2_[7].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.87f),GFX_BOX2D_POS_Y(platform_.height()*0.08f),
//			GFX_BOX2D_SIZE(platform_.width()*0.12f),GFX_BOX2D_SIZE(platform_.height()*0.08f));//top right	
//		platforms2_[8].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.85f),GFX_BOX2D_POS_Y(platform_.height()*0.8f),
//			GFX_BOX2D_SIZE(platform_.width()*0.08f),GFX_BOX2D_SIZE(platform_.height()*0.08f));//right outcrop
//		platforms2_[9].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.91f),GFX_BOX2D_POS_Y(platform_.height()*0.7f),
//			GFX_BOX2D_SIZE(platform_.width()*0.08f),GFX_BOX2D_SIZE(platform_.height()*0.35f));//right side	
//	}
//	//set object type
//	for(int i = 0;i < PLATFORM_NUM2;i++)
//	{
//		platforms2_[i].setType(GameObject::PLATFORM);
//	}
//	
//	//create pickups on ceiling
//	pickUp2_[0].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.3f),GFX_BOX2D_POS_Y(platform_.height()*0.85f));
//	pickUp2_[1].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.5f),GFX_BOX2D_POS_Y(platform_.height()*0.75f));
//	pickUp2_[2].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.65f),GFX_BOX2D_POS_Y(platform_.height()*0.1f));
//	pickUp2_[3].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.5f),GFX_BOX2D_POS_Y(platform_.height()*0.3f));
//	pickUp2_[4].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.25f),GFX_BOX2D_POS_Y(platform_.height()*0.4f));
//	pickUp2_[5].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.4f),GFX_BOX2D_POS_Y(platform_.height()*0.1f));
//	pickUp2_[6].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.75f),GFX_BOX2D_POS_Y(platform_.height()*0.5f));
//	pickUp2_[7].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.95f),GFX_BOX2D_POS_Y(platform_.height()*0.25f));
//
//	//create plants
//	//tunnel blockers
//	//plant2_[0].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.16f),GFX_BOX2D_POS_Y(platform_.height()*0.82f),GFX_BOX2D_SIZE(20)
//	//	,GFX_BOX2D_SIZE(platform_.height()*0.12f));
//	//plant2_[1].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.29f),GFX_BOX2D_POS_Y(platform_.height()*0.82),GFX_BOX2D_SIZE(20),
//	//	GFX_BOX2D_SIZE(platform_.height()*0.12f));
//	////treasure holders
//	//plant2_[2].CreateStaticBody(world_,GFX_BOX2D_POS_X(30),GFX_BOX2D_POS_Y(platform_.height()*0.325),GFX_BOX2D_SIZE(20),
//	//	GFX_BOX2D_SIZE(20));//left
//	//plant2_[3].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.28f),GFX_BOX2D_POS_Y(platform_.height()*0.325),GFX_BOX2D_SIZE(20),
//	//	GFX_BOX2D_SIZE(20));//right
//	//plant2_[4].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.16f),GFX_BOX2D_POS_Y(30),GFX_BOX2D_SIZE(20)
//	//	,GFX_BOX2D_SIZE(20));//top
//
//	//set to plant type
//	for(int j = 0; j < PLANT_NUM2;j++)
//	{
//		plant2_[j].setType(GameObject::PLANT);
//	}
//
//	//create spike
//	//floor
//	spike2_[0].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.6f),GFX_BOX2D_POS_Y(platform_.height()-15),
//		GFX_BOX2D_SIZE(platform_.width()*0.25f),GFX_BOX2D_SIZE(5));
//	//top left
//	//spike2_[1].CreateStaticBody(world_,GFX_BOX2D_POS_X(0),GFX_BOX2D_POS_Y(0),
//	//	GFX_BOX2D_SIZE(40),GFX_BOX2D_SIZE(40));
//	//spike2_[1].set_rotation(40);
//	////top right
//	//spike2_[2].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.32f),GFX_BOX2D_POS_Y(5),
//	//	GFX_BOX2D_SIZE(40),GFX_BOX2D_SIZE(40));
//	//spike2_[2].set_rotation(40);
//	////bottom right
//	//spike2_[3].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.31f),GFX_BOX2D_POS_Y(platform_.height()*0.545f),
//	//	GFX_BOX2D_SIZE(25),GFX_BOX2D_SIZE(25));
//	//spike2_[3].set_rotation(40);
//
//	//set to spike type
//	for(int k = 0; k < SPIKE_NUM2; k++)
//	{
//		spike2_[k].set_colour(0xff0000ff);//set colour to red
//		spike2_[k].setType(GameObject::SPIKE);
//	}
//}

//void Level_2::Restart()
//{
//	//destroy objects
//	Destroy(player2_);//player
//	Destroy(enemy2_);//enemy
//
//	//destroy pickups
//	for ( int y = 0; y < PICKUP_NUM2; y++)
//	{
//		//reset for another play
//		pickUp2_[y].dead = true;
//		pickUp2_[y].spawned = false;
//
//		Destroy(pickUp2_[y]);
//	}
//
//	//for(int s = 0; s < PLANT_NUM2; s++)
//	//{
//	//	//destroy plants
//	//	Destroy(plant2_[s]);
//	//}
//
//	score_ = 0;
//
//	//create new versions at starting position
//	CreateObjects();
//
//	gameOver_ = false;
//}
//
//void Level_2::PlantPickUps()
//{
//	////left plant
//	//if(plant2_[2].destroyed == true && pickUp2_[6].spawned == false)
//	//{
//	//	pickUp2_[6].Create_pickup(world_,GFX_BOX2D_POS_X(30),GFX_BOX2D_POS_Y(platform_.height()*0.325));
//	//	pickUp2_[6].spawned = true;
//	//}
//
//	////right plant
//	//if(plant2_[3].destroyed == true && pickUp2_[7].spawned == false)
//	//{
//	//	pickUp2_[7].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.26f),GFX_BOX2D_POS_Y(platform_.height()*0.325));
//	//	pickUp2_[7].spawned = true;
//	//}
//
//	////top plant
//	//if(plant2_[4].destroyed == true && pickUp2_[8].spawned == false)
//	//{
//	//	pickUp2_[8].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.16f),GFX_BOX2D_POS_Y(30));
//	//	pickUp2_[8].spawned = true;
//	//}
//
//}