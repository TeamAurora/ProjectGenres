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
	//Load textures using application_->LoadTextureFromPNG("texturename.png")

	// single object texture loaded directly into objects
	arrow_.set_texture(application_->LoadTextureFromPNG("arrow.png"));

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

APPSTATE Level_1::InputLoop(const abfw::SonyController* controller)
{
	//so player can't move when dead
	if(player_.dead == false)
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

	return LEVEL_1;
}

void Level_1::CreateObjects()
{
	//make moving objects
	player_.Create_Player(world_, GFX_BOX2D_POS_X(platform_.width()*0.15),GFX_BOX2D_POS_Y(platform_.height()*0.85));

	//set game objects' textures
	player_.set_texture(playerTex);

	Enemy enemy_ = new Enemy();
	enemy.Create_Enemy(world_, GFX_BOX2D_POS_X(platform_.width()*0.25f),GFX_BOX2D_POS_Y(platform_.height()*0.2));
	enemy.gravity = b2Vec2(0,-10);

	enemies_.push_back(enemy);

	for(int enemyindex = 0; enemyindex < enemies_.size(); enemyindex++)
	{
		enemies_[enemyindex].set_texture(enemyMove);
	}


	//make non moving objects
	//create walls
	if(platforms_[0].dead == true)//check if platforms has been created only needs to check one as they are all done at the same time
	{
		platformWidth_ = GFX_BOX2D_SIZE(10);//width of screen boundry platforms

		//boundries
		platforms_[0].CreateStaticBody(world_,GFX_BOX2D_POS_X(0),GFX_BOX2D_POS_Y(0),GFX_BOX2D_SIZE(platform_.width()),platformWidth_);//roof
		platforms_[1].CreateStaticBody(world_,GFX_BOX2D_POS_X(0),GFX_BOX2D_POS_Y(platform_.height()),GFX_BOX2D_SIZE(platform_.width()),platformWidth_);//ground
		platforms_[2].CreateStaticBody(world_,GFX_BOX2D_POS_X(0),GFX_BOX2D_POS_Y(platform_.height()),platformWidth_,GFX_BOX2D_SIZE(platform_.height()));//left wall
		platforms_[3].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()),GFX_BOX2D_POS_Y(0),platformWidth_,GFX_BOX2D_SIZE(platform_.height()));//right wall
		//level parts
		platforms_[4].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.45f),GFX_BOX2D_POS_Y(platform_.height()*0.9f),
			GFX_BOX2D_SIZE(platform_.width()*0.11f),GFX_BOX2D_SIZE(platform_.height()*0.325f));	//centre
		platforms_[5].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.175f),GFX_BOX2D_POS_Y(platform_.height()*0.32f),
			GFX_BOX2D_SIZE(platform_.width()*0.175f),GFX_BOX2D_SIZE(platform_.height()*0.06f));//top left
		platforms_[6].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.82f),GFX_BOX2D_POS_Y(platform_.height()*0.326f),
			GFX_BOX2D_SIZE(platform_.width()*0.175f),GFX_BOX2D_SIZE(platform_.height()*0.06f));//top right
		platforms_[7].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.6f),GFX_BOX2D_POS_Y(platform_.height()*0.90f),
			GFX_BOX2D_SIZE(platform_.width()*0.15f),GFX_BOX2D_SIZE(platform_.height()*0.15f));//slope
		platforms_[7].set_rotation(0.785398163);//45 degree angle in radians
	}

	//set object type
	for(int i = 0;i < PLATFORM_NUM;i++)
	{
		platforms_[i].setType(GameObject::PLATFORM);
	}
	
	//create pickups 
	//top right
	pickUp_[0].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.85f),GFX_BOX2D_POS_Y(platform_.height()*0.15f));
	pickUp_[1].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.9f),GFX_BOX2D_POS_Y(platform_.height()*0.15f));
	pickUp_[2].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.95f),GFX_BOX2D_POS_Y(platform_.height()*0.15f));

	//bottom right
	pickUp_[3].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.95f),GFX_BOX2D_POS_Y(platform_.height()*0.55f));
	pickUp_[4].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.95f),GFX_BOX2D_POS_Y(platform_.height()*0.65f));
	pickUp_[5].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.95f),GFX_BOX2D_POS_Y(platform_.height()*0.75f));
	pickUp_[6].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.95f),GFX_BOX2D_POS_Y(platform_.height()*0.85f));
	//top left
	pickUp_[7].Create_pickup(world_,GFX_BOX2D_POS_X(platform_.width()*0.05f),GFX_BOX2D_POS_Y(platform_.height()*0.05f));

	//create plants
	//tunnel blocker
	plant_[0].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.7f),GFX_BOX2D_POS_Y(platform_.height()*0.145f),GFX_BOX2D_SIZE(20)
		,GFX_BOX2D_SIZE(platform_.height()*0.13f));
	plant_[0].rotated = false;

	//set to plant type
	for(int j = 0; j < PLANT_NUM;j++)
	{
		plant_[j].setType(GameObject::PLANT);
		plant_[j].setAnimation();//set up for death animation
	}

	//create spike
	//top left
	spike_[0].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.38f),GFX_BOX2D_POS_Y(platform_.height()*0.32f),
		GFX_BOX2D_SIZE(25),GFX_BOX2D_SIZE(platform_.height()*0.05f));
	//slope
	spike_[1].CreateStaticBody(world_,GFX_BOX2D_POS_X(platform_.width()*0.66f),GFX_BOX2D_POS_Y(platform_.height()*0.8),
		GFX_BOX2D_SIZE(platform_.width()*0.16),GFX_BOX2D_SIZE(30));
	spike_[1].set_rotation(0.785398163);

	//set to spike type
	for(int k = 0; k < SPIKE_NUM; k++)
	{
		spike_[k].set_colour(0xff0000ff);//set colour to red
		spike_[k].setType(GameObject::SPIKE);
	}
}

void Level_1::UpdateGameObjects(const float& ticks_, const int& frame_counter_)
{
	// Any per-frame or per-tick updating for game objects should be done here
	
	//update player if alive
	if(player_.dead != true)
	{
		player_.Update(ticks_, gameOver_,false);
		arrow_.set_position(player_.currentPos.x, player_.currentPos.y, 0.0f);	
		arrow_.set_rotation(player_.currentRayAngle - abfw::DegToRad(45.0f));
	}
	else if (player_.deadAnim == false)//play death animation
	{
		player_.Update(ticks_, gameOver_,false);
	}
	else
	{
		//Restart();
	}

	
	arrow_.set_position(player_.currentPos.x,player_.currentPos.y,0.0f);	
	arrow_.set_rotation(player_.currentRayAngle);
	
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
	if(enemy_.dead != true)
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
	switch(enemy_.meleeState_)
	{
		case Enemy::MOVING:
			enemy_.set_texture(enemyMove);
			break;
		case Enemy::DEAD:
			enemy_.set_texture(enemyDeath);
			break;
	};

	//spawn extra pickup from certain plants
	//PlantPickUps();

	//win condition
	if(score_ == PICKUP_NUM)
	{
		gameOver_ = true;
	}	
}

void Level_1::Restart()
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

void Level_1::PlantPickUps()
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