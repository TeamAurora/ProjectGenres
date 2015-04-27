#include "level_2.h"
#include "game_application.h"

Level_2::Level_2(abfw::Platform& platform, const GameApplication* application, abfw::AudioManager* audio_manager) :
	LevelState(platform, application, audio_manager, LEVEL_2)
{
}

Level_2::~Level_2()
{
}

void Level_2::LoadAssets()
{
	LoadMap("level_2.tmx");

	// state-level textures 
	//player
	playerArrow = application_->LoadTextureFromPNG("player_arrow.png");
	playerTex = application_->LoadTextureFromPNG("player_animations.png");
	rotPlayerTex = application_->LoadTextureFromPNG("player_animations_rotated.png");
	playerIdle = application_->LoadTextureFromPNG("player_idle.png");	
	rotPlayerIdle = application_->LoadTextureFromPNG("player_idle_rotated.png");
	playerDeath = application_->LoadTextureFromPNG("player_death.png");
	rotPlayerDeath = application_->LoadTextureFromPNG("player_death_rotated.png");
	playerJump = application_->LoadTextureFromPNG("player_jump.png");
	rotPlayerJump = application_->LoadTextureFromPNG("player_jump_rotated.png");
	playerAttack = application_->LoadTextureFromPNG("player_attack.png");
	rotPlayerAttack = application_->LoadTextureFromPNG("player_attack_rotated.png");

	////enemy
	enemyMove = application_->LoadTextureFromPNG("enemy_melee_move.png");
	enemyDeath = application_->LoadTextureFromPNG("enemy_melee_death.png");
	enemyAttack = application_->LoadTextureFromPNG("enemy_melee_attack.png");
	plantWallTex = application_->LoadTextureFromPNG("plant_wall_death.png");
}

APPSTATE Level_2::InputLoop(const abfw::SonyController* controller)
{
	//so player can't move when dead
	if(player_.dead != true)
	{
		player_.Player_Input(controller);
	}

	return LEVEL_2;
}

void Level_2::UpdateGameObjects(const float& ticks_, const int& frame_counter_)
{
	// Any per-frame or per-tick updating for game objects should be done here
	
	//update player if alive
	if(player_.dead != true)
	{
		player_.Update(ticks_, gameOver_, true);
		arrow_.set_position(player_.position().x, player_.position().y, 0.0f);	
		arrow_.set_rotation(player_.currentRayAngle - abfw::DegToRad(90.0f));
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
	if(player_.attacking == true && blade_.disabled == true)
	{
		blade_.Create(world_, player_);
		attackTime = 0;
	}
	else if (blade_.disabled == false && attackTime > 20)//destroy blade after a certain time
	{
		blade_.DestroyBody();
		blade_.disabled = true;
	}
	else if(attackTime < 20)
	{
		blade_.Update(ticks_,player_);
		attackTime += (ticks_*20);
	}

	//update enemy if alive
	if(enemy_.dead != true)
	{
		enemy_.Update_Enemy(ticks_, b2Vec2(player_.position().x, player_.position().y), false);
		reloadTime  += (ticks_*20);

		if(reloadTime > 50 && bullet_.created == false)//shoot a bullet at intervals
		{
			enemy_.shooting = true;

			if(enemy_.shotFired)//shoot animation done
			{
				//bullet_.CreateBullet(world_,enemy_.x,enemy_.y, enemy_.gravity,  b2Vec2(player_.position().x, player_.position().y));//fire
				//reset 
				reloadTime = 0;
				enemy_.shotFired = false;
			}
		}
		else
		{
			enemy_.shooting = false;
		}
	}
	else if(enemy_.deadAnim == false)//play death animation
	{
		enemy_.Update_Enemy(ticks_, b2Vec2(player_.position().x, player_.position().y), false);
	}
	else
	{
		DestroyBody(enemy_);//remove enemys
	}

	if(bullet_.dead != true && bullet_.created == true)
	{
		bullet_.Update(ticks_);
	}
	else if (bullet_.dead == true)
	{
		DestroyBody(bullet_);
		bullet_.created = false;
	}

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
				player_.set_texture(rotPlayerAttack);
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
				player_.set_texture(playerAttack);
				break;
			case Player::DEAD:
				player_.set_texture(playerDeath);
				break;
			case Player::JUMPING:
				player_.set_texture(playerJump);
				break;
		};
	}

	switch(enemy_.shooterState_)
	{
		case Enemy::IDLE:
			enemy_.set_texture(enemyShooting);
			break;
		case Enemy::SHOOTING:
			enemy_.set_texture(enemyShooting);
			break;
		case Enemy::DEAD:			
			enemy_.set_texture(shooterDeath);
			break;
	};
}

void Level_2::CreateObjects()
{
	enemy_.Create_Enemy(world_, GFX_BOX2D_POS_X(platform_.width()*0.55f),GFX_BOX2D_POS_Y(platform_.height()*0.1));
	enemy_.gravity = b2Vec2(0,10);

	//make moving objects
	player_.Create_Player(world_, GFX_BOX2D_POS_X(4352.0f),GFX_BOX2D_POS_Y(2944.0f));
	application_->player_camera_->MoveTo(abfw::Vector2(player_.position().x - (platform_.width() / 2.0f), player_.position().y - (platform_.height() / 2.0f)));

	//set game objects' textures
	player_.set_texture(playerTex);
	arrow_.set_texture(playerArrow);
	arrow_.set_width(512.0f);
	arrow_.set_height(512.0f);
	blade_.Create(world_, player_);

	for(int pickup = 0; pickup < pickups_.size(); pickup++)
	{
		// check if there is no body before recreation
		if(pickups_[pickup].body_ == NULL)
		{
			pickups_[pickup].CreateBody(world_);
		}
	}

	// put plant walls here

	start_time_ = std::clock();
}

void Level_2::Restart()
{
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
}