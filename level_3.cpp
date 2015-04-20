#include "level_3.h"
#include "game_application.h"

Level_3::Level_3(abfw::Platform& platform, const GameApplication* application, abfw::AudioManager* audio_manager) :
	LevelState(platform, application, audio_manager, LEVEL_1)
{
}

Level_3::~Level_3()
{
}

void Level_3::LoadAssets()
{
	//Load textures using application_->LoadTextureFromPNG("texturename.png")

	LoadMap("level3.tmx");

	// single object texture loaded directly into objects
	playerArrow = application_->LoadTextureFromPNG("arrow.png");

	// state-level textures 
	// player
	playerTex = application_->LoadTextureFromPNG("Robot_animations.png");
	rotPlayerTex = application_->LoadTextureFromPNG("Robot_animations_rotated.png");
	playerIdle = application_->LoadTextureFromPNG("Robot_Animation_Idle.png");	
	rotPlayerIdle = application_->LoadTextureFromPNG("Robot_Animation_Idle_rot.png");
	playerDeath = application_->LoadTextureFromPNG("Robot_Animation_death.png");
	rotPlayerDeath = application_->LoadTextureFromPNG("Robot_Animation_death_rot.png");
	playerJump = application_->LoadTextureFromPNG("Robot_AnimatioN_JUMP.png");
	rotPlayerJump = application_->LoadTextureFromPNG("Robot_AnimatioN_JUMP_rot.png");

	//enemy
	enemyMove = application_->LoadTextureFromPNG("Enemy_walk.png");
	enemyDeath = application_->LoadTextureFromPNG("Enemy_death_animation.png");

	////pickups
	red_pickup_texture_ = application_->LoadTextureFromPNG("pickup_red.png");
	blue_pickup_texture_ = application_->LoadTextureFromPNG("pickup_blue.png");
	yellow_pickup_texture_ = application_->LoadTextureFromPNG("pickup_yellow.png");
	green_pickup_texture_ = application_->LoadTextureFromPNG("pickup_green.png");
}

APPSTATE Level_3::InputLoop(const abfw::SonyController* controller)
{
	//so player can't move when dead
	if(player_.dead == false)
	{
		player_.Player_Input(controller);
	}

	return LEVEL_1;
}

void Level_3::CreateObjects()
{
	//make moving objects
	player_.Create_Player(world_, GFX_BOX2D_POS_X(4352.0f),GFX_BOX2D_POS_Y(2944.0f));

	//set game objects' textures
	player_.set_texture(playerTex);
	arrow_.set_texture(playerArrow);
	arrow_.set_width(256.0f);
	arrow_.set_height(256.0f);

	// Enemies
	/*Enemy enemy = Enemy();
	enemy.Create_Enemy(world_, GFX_BOX2D_POS_X(platform_.width()*0.25f),GFX_BOX2D_POS_Y(platform_.height()*0.2));
	enemy.gravity = b2Vec2(0,-10);

	enemies_.push_back(enemy);
	
	for(int enemyindex = 0; enemyindex < enemies_.size(); enemyindex++)
	{
		enemies_[enemyindex].set_texture(enemyMove);
	}*/
}

void Level_3::UpdateGameObjects(const float& ticks_, const int& frame_counter_)
{
	// Any per-frame or per-tick updating for game objects should be done here
	
	//update player if alive
	if(player_.dead != true)
	{
		player_.Update(ticks_, gameOver_, false);
		arrow_.set_position(player_.position().x, player_.position().y, 0.0f);	
		arrow_.set_rotation(player_.currentRayAngle - abfw::DegToRad(90.0f));
	}
	else if (player_.deadAnim == false)//play death animation
	{
		player_.Update(ticks_, gameOver_, false);
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
		blade_.Update(ticks_,player_);
		attackTime += (ticks_*20);
	}

	//update enemy if alive
	/*if(enemy_.dead != true)
	{
		enemy_.Update_Enemy(ticks_, player_.currentPos, true);
	}
	else if (enemy_.deadAnim == false)//play death animation
	{
		enemy_.Update_Enemy(ticks_, player_.currentPos, true);
	}
	else
	{
		Destroy(enemy_); //remove enemys
	}*/

	//remove from game if collected
	/*for(int i = 0;i < PICKUP_NUM;i++)
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

	//check for destroyed plants
	for(int g = 0; g < PLANT_NUM;g++)
	{
		if(plant_[g].dead == true && plant_[g].deadAnim == false)
		{
			plant_[g].deadAnim = plant_[g].Animate(ticks_,true);
		}
		else if (plant_[g].deadAnim == true)
		{
			Destroy(plant_[g]);
		}
	}*/

	//change sprite for horizontal or vertical movement
	if (player_.horizontal == false)
	{
		//change textures
		switch(player_.state_)
		{
			case Player::IDLE:
					player_.set_texture(rotPlayerIdle);
					break;
			case Player::RUNNING:
					player_.set_texture(rotPlayerTex);
					break;			 
			case Player::ATTACKING:

				break;
			case Player::DEAD:
				player_.set_texture(rotPlayerDeath);
				break;
			case Player::JUMPING:
				player_.set_texture(rotPlayerJump);
				break;
		};
	}
	else
	{
		//change textures
		switch(player_.state_)
		{
			case Player::IDLE:
					player_.set_texture(playerIdle);
					break;
			case Player::RUNNING:
					player_.set_texture(playerTex);
					break;			 
			case Player::ATTACKING:

				break;
			case Player::DEAD:
				player_.set_texture(playerDeath);
				break;
			case Player::JUMPING:
				player_.set_texture(playerJump);
				break;
		};
	}

	//change enemy textures
	/*switch(enemy_.meleeState_)
	{
		case Enemy::MOVING:
			enemy_.set_texture(enemyMove);
			break;
		case Enemy::DEAD:
			enemy_.set_texture(enemyDeath);
			break;
	};*/

	//spawn extra pickup from certain plants
	//PlantPickUps();

	//win condition
	/*if(score_ == PICKUP_NUM)
	{
		gameOver_ = true;
	}	*/
}

void Level_3::Restart()
{
	//destroy objects
	Destroy(player_);//player
	//Destroy(enemy_);//enemy

	//destroy pickups
	for ( int pickupindex = 0; pickupindex < pickups_.size(); pickupindex++)
	{
		//reset for another play
		pickups_[pickupindex].dead = true;
		pickups_[pickupindex].spawned = false;

		Destroy(pickups_[pickupindex]);
	}

	for(int plantsindex = 0; plantsindex < plants_.size(); plantsindex++)
	{
		//destroy plants
		Destroy(plants_[plantsindex]);
	}

	score_ = 0;

	//create new versions at starting position
	CreateObjects();

	gameOver_ = false;
}