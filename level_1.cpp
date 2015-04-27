#include "level_1.h"
#include "game_application.h"

Level_1::Level_1(abfw::Platform& platform, const GameApplication* application, abfw::AudioManager* audio_manager) :
	LevelState(platform, application, audio_manager, LEVEL_1)
{
}

Level_1::~Level_1()
{
}

void Level_1::LoadAssets()
{
	LoadMap("level_1.tmx");

	// player
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
}

APPSTATE Level_1::InputLoop(const abfw::SonyController* controller)
{
	//so player can't move when dead
	if(player_.dead == false)
	{
		player_.Player_Input(controller);
	}

	return LEVEL_1;
}

void Level_1::CreateObjects()
{
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

	start_time_ = std::clock();
}

void Level_1::UpdateGameObjects(const float& ticks_, const int& frame_counter_)
{
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
		}
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
		}
	}

	
	for(int pickup = 0; pickup < pickups_.size(); pickup++)
	{
		pickups_[pickup].UpdatePosition();
		if(pickups_[pickup].collided == true)
		{
			switch(pickups_[pickup].pickup_type_)
			{
			case PickUp::RED:
				score_ += 30;
				break;
			case PickUp::BLUE:
				score_ += 60;
				break;
			case PickUp::YELLOW:
				score_ += 90;
				break;
			case PickUp::GREEN:
				score_ += 120;
				gameOver_ = true;
				break;
			}
			collectables_++;
			pickups_[pickup].DestroyBody();
			pickups_[pickup].collided = false;
		}
	}
}

void Level_1::Restart()
{
	//destroy objects
	player_.DestroyBody();
	blade_.DestroyBody();

	//destroy pickups
	for ( int pickupindex = 0; pickupindex < pickups_.size(); pickupindex++)
	{
		//reset for another play
		if(pickups_[pickupindex].dead == true)
		{
			pickups_[pickupindex].DestroyBody();
		}
	}

	score_ = 0;
	collectables_ = 0;

	//create new versions at starting position
	CreateObjects();

	gameOver_ = false;
}