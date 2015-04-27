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
	LoadMap("level_3.tmx");

	playerDeath = application_->LoadTextureFromPNG("player_death.png");
	playerFlying = application_->LoadTextureFromPNG("player_floating.png");
	playerAttack = application_->LoadTextureFromPNG("player_floating_attack.png");

	//enemy
	shooterFiring = application_->LoadTextureFromPNG("enemy_ranged_firing.png");
	shooterDeath = application_->LoadTextureFromPNG("enemy_ranged_death.png");
	shooterProjectile = application_->LoadTextureFromPNG("enemy_projectile.png");
}

APPSTATE Level_3::InputLoop(const abfw::SonyController* controller)
{
	//so player can't move when dead
	if(player_.dead == false)
	{
		player_.Player_Input(controller);
	}

	return LEVEL_3;
}

void Level_3::CreateObjects()
{
	//enemy_.Create_Enemy(world_, GFX_BOX2D_POS_X(),GFX_BOX2D_POS_Y(platform_.height()*0.1));
	//enemy_.gravity = b2Vec2(0,10);

	//make moving objects
	player_.Create_Player(world_, GFX_BOX2D_POS_X(2880.0f),GFX_BOX2D_POS_Y(5312.0f));

	//set game objects' textures
	player_.set_texture(playerFlying);
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

	for(int plant = 0; plant < plants_.size(); plant++)
	{
		// check if there is no body before recreation
		if(plants_[plant].body_ == NULL)
		{
			plants_[plant].CreateBody(world_);
		}
	}

	start_time_ = std::clock();
}

void Level_3::Restart()
{
	//destroy objects
	player_.DestroyBody();
	blade_.DestroyBody();
	enemy_.DestroyBody();

	//destroy pickups
	for ( int pickupindex = 0; pickupindex < pickups_.size(); pickupindex++)
	{
		//reset for another play
		if(pickups_[pickupindex].dead == true)
		{
			pickups_[pickupindex].DestroyBody();
		}
	}

	//destroy pickups
	for ( int plantindex = 0; plantindex < plants_.size(); plantindex++)
	{
		//reset for another play
		if(plants_[plantindex].dead == true)
		{
			plants_[plantindex].DestroyBody();
		}
	}

	score_ = 0;
	collectables_ = 0;

	//create new versions at starting position
	CreateObjects();

	gameOver_ = false;
}

void Level_3::UpdateGameObjects(const float& ticks_, const int& frame_counter_)
{	
	//update player if alive
	if(player_.dead != true)
	{
		player_.Update(ticks_, gameOver_, true);
		arrow_.set_position(player_.position().x, player_.position().y, 0.0f);	
		arrow_.set_rotation(player_.currentRayAngle - abfw::DegToRad(90.0f));
	}
	else if (player_.deadAnim == false)//play death animation
	{
		player_.Update(ticks_, gameOver_, true);
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

	if(enemy_.dead != true)
	{
		enemy_.Update_Enemy(ticks_, b2Vec2(player_.position().x, player_.position().y), false);
		reloadTime  += (ticks_*20);

		if(reloadTime > 50 && bullet_.created == false)//shoot a bullet at intervals
		{
			enemy_.shooting = true;

			if(enemy_.shotFired)//shoot animation done
			{
				bullet_.CreateBullet(world_,enemy_.position().x,enemy_.position().y, enemy_.gravity,  b2Vec2(player_.position().x, player_.position().y));//fire
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
		enemy_.DestroyBody();//remove enemys
	}

	if(bullet_.dead != true && bullet_.created == true)
	{
		bullet_.Update(ticks_);
	}
	else if (bullet_.dead == true)
	{
		bullet_.DestroyBody();
		bullet_.created = false;
	}

	//change textures
	switch(player_.state_)
	{
		case Player::FLYING:
			player_.set_texture(playerFlying);
			break;			 
		case Player::ATTACKING:
			player_.set_texture(playerAttack);
			break;
		case Player::DEAD:
			player_.set_texture(playerDeath);
			break;
	};

	switch(enemy_.shooterState_)
	{
		case Enemy::IDLE:
			enemy_.set_texture(shooterFiring);
			break;
		case Enemy::SHOOTING:
			enemy_.set_texture(shooterFiring);
			break;
		case Enemy::DEAD:			
			enemy_.set_texture(shooterDeath);
			break;
	};

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

	for(int plant = 0; plant < plants_.size(); plant++)
	{
		plants_[plant].UpdatePosition();
		if(plants_[plant].collided == true)
		{
			score_ += 90;
			plants_[plant].DestroyBody();
			plants_[plant].collided = false;
		}
	}
}