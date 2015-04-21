#include "Player.h"
#include <input/sony_controller_input_manager.h>
#include "box2d_helpers.h"

/////John//////
Player::Player()
{
	// set gameobject type
	type_ = PLAYER;

	// start idle
	state_ = IDLE;

	//set values for variables
	move_v = 6;
	damage = 1.25;

	//texture coords
	uv_x = 0.0f;
	uv_y = 0.0f;
	uv_width = 0.03125f;
	uv_height = 0.5f;

	//player states
	attacking = false;
	hurting = false;
	move_right = false;
	horizontal = true;
	moveUp = true;

	deadAnim = false;//has death animation played

	//on stick axis
	jumpCutOff = 0.8;
	moveCutOff = 0.6;

	//jump angle in radians
	currentRayAngle = 0; //5.235987756 ;//300 radians
	rayLength = 5.0f;
	setGravity(0);//down
}

Player::~Player()
{
}

void Player::Create_Player(b2World* world_, float x, float y)
{
	//varaibles that need reset if player has been killed
	dead = false;
	destroyed = false;
	setMaxHealth(100.0f);
	setHealth(max_health());	
	state_ = IDLE;
	deadAnim = false;
	setFlyingWidth = false;

	// sets initial gravity to be down
	gravity = b2Vec2(0.0f, -10.0f);
	gDir = DOWN;

	//create body
	bodyInitialPosition.x = x;//
	bodyInitialPosition.y = y;//

	body_half_width = 0.35f;
	body_half_height = 0.35f;

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.x = bodyInitialPosition.x;
	bodyDef.position.y = bodyInitialPosition.y;
	bodyDef.fixedRotation = true;//stops body rotating 
	body_ = world_->CreateBody(&bodyDef);
	physicsengine_ = BOX2D;			// changes to box2d physics for this object

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(body_half_width, body_half_height);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 2.0f;//set mass
	body_->ResetMassData();//it only sets new value after this is called
	fixtureDef.friction = 0.95f;
	fixtureDef.restitution = 0.1f; // not bouncy
	body_->CreateFixture(&fixtureDef);

	body_->SetUserData(this);

	//sprite set up
	//set sprite size to match body
	set_width(BOX2D_GFX_SIZE(2 * body_half_width));
	set_height(BOX2D_GFX_SIZE(2 * body_half_height));

	//uv height, width and position
	set_uv_height(uv_height);
	set_uv_width(uv_width);
	set_uv_position(abfw::Vector2(uv_x, uv_y));
}

void Player::Update(const float& ticks, bool gameOver, bool flying)
{
	bool result;
	
	float new_x = BOX2D_GFX_POS_X(body_->GetPosition().x);
	float new_y = BOX2D_GFX_POS_Y(body_->GetPosition().y);

	//update sprite position to match body
	set_position(abfw::Vector3(new_x, new_y, 0.f));
	set_rotation(-body_->GetAngle());

	currentPos = b2Vec2(new_x, new_y);//gives access to current position

	mflying = flying;//check what animation should be done

	if(setFlyingWidth == false && mflying == true)
	{
		set_uv_width(0.0625f);
		set_uv_height(1);
		setFlyingWidth = true;
	}

	OBJECTSTATE prevState = state_;

	//damage player
	if(hurting && health() > 0)
	{
		updateHealth(-damage);	
		MoveBy(knockbackForce_.x,knockbackForce_.y);
	}

	//die
	if(health() <= 0)
	{
		dead = true;
	}	

	//set player's axis
	if(gDir == UP || gDir == DOWN)
	{
		horizontal = true;
	}
	else
	{
		horizontal = false;
	}

	if(mflying == false)
	{
		changeState();//check variables and change player's state

		//flip facing back the right way after jump has landed
		if(prevState == INAIR &&  state_ != INAIR)
		{
			if(move_right)
			{
				move_right = false;
			}
			else
			{
				move_right = true;
			}

			if(moveUp)
			{
				moveUp = false;
			}
			else
			{
				moveUp = true;
			}
		}


		//set up animations for each state
		if(state_ != prevState)
		{
			switch(state_)
			{
				case IDLE:
						idleAnimation();
						break;
				case RUNNING:
						runningAnimation();
						break;			 
				case ATTACKING:		
						attackAnimation();
						break;
				case DEAD:
						deadAnimation();
					break;
				case JUMPING:
					break;
			};
		}
	}
	else//flying level
	{
		//change state
		if (attacking == true)
		{
			state_ = ATTACKING;
		}
		else if(dead == true && attacking == false)
		{
			state_ = DEAD;
			body_->SetLinearVelocity(b2Vec2(0,0));//stop all movement
		}
		else
		{
			state_ = FLYING;
		}


		//set up animations for each state
		if(state_ != prevState)
		{
			
			switch(state_)
			{
				case ATTACKING:
						attackAnimation();
						break;
				case FLYING:
						break;
				case DEAD:
						deadAnimation();
						break;
			};
		}
	}
			
	// play the animation
	if(horizontal == true)
	{
		result = Animate(ticks, move_right);
	}
	else
	{
		result = Animate(ticks, moveUp);
	}

	//check for animations that play once
	if(state_ == DEAD)
	{
		deadAnim = result;//death animation has played
	}

	//jump animation has played
	if(state_ == JUMPING && result == true)
	{
		state_ = INAIR;
	}

	//attack animation has played
	if(state_ == ATTACKING && result == true)
	{
		attacking = false;
		if(mflying)
		{
			//reset uv
			set_uv_width(0.0625f);
			set_uv_position(abfw::Vector2 (0.0f,0.0f));
		}
		//reset sprite size
		set_width(BOX2D_GFX_SIZE(2 * body_half_width));
		set_height(BOX2D_GFX_SIZE(2 * body_half_height));
	}
	

	if(!mflying)
	{
		if(state_ != JUMPING && state_ != INAIR)//no change in movement during jump
		{
			//apply gravity
			body_->ApplyForceToCenter(gravity);
		}
	}
	else
	{		
		//apply gravity
		body_->ApplyForceToCenter(gravity);
	}
}

void Player::Player_Input(const abfw::SonyController* controller)
{

	// check we have a valid controller object (one that isn't NULL)
	if (controller)
	{	

////Rebecca//////////
		// Sets current gravity manipulation via player input
		//jetpack flight
		if(mflying == true)
		{
			gDir = DOWN;
			if (controller->right_stick_y_axis() < -jumpCutOff)//up
			{
				gravity = b2Vec2(0.0f, 10.0f);
				state_ = FLYING;
				flyAnimation();
			}
			else if (controller->right_stick_x_axis() > jumpCutOff)//right
			{
				gravity = b2Vec2(10.0f, 0.0f);
				state_ = FLYING;
				move_right = true;
				flyAnimation();
			}
			else if (controller->right_stick_x_axis() < -jumpCutOff)//left
			{
				gravity = b2Vec2(-10.0f, 0.0f);
				state_ = FLYING;
				move_right = false;
				flyAnimation();
			}
			else if (controller->right_stick_y_axis()> jumpCutOff)//down
			{
				gravity = b2Vec2(0.0f, -10.0f);
				state_ = FLYING;
				flyAnimation();
			}
		}
		else//wall jump
		{
			// change angle of projected jump
			float xaxisval = controller->right_stick_x_axis();
			float yaxisval = controller->right_stick_y_axis();
			currentRayAngle = atan2(xaxisval, -yaxisval); //* FRAMEWORK_RAD_TO_DEG;

			// jump mechanic
			if (controller->buttons_pressed() & ABFW_SONY_CTRL_R2 && state_ != INAIR)
			{
				//calls to set up animtaion for the jump
				state_ = JUMPING;
				jumpAnimation(xaxisval, yaxisval);

				//set_rotation(currentRayAngle);
			

				//state_ = INAIR;
				b2Vec2 angle = b2Vec2(sinf(currentRayAngle), cosf(currentRayAngle));
				b2Vec2 impulse = b2Vec2(rayLength * angle);

				body_->ApplyLinearImpulse(impulse,body_->GetWorldCenter());
			}
		}

		// checks what gravity is currently applied and dictates what movement
		// can be used ie. x axis movement on ground/ceiling, y axis movement on
		// walls
		if (gDir == DOWN || gDir == UP)
		{
			if (controller->left_stick_x_axis() > moveCutOff)//right
			{
				move = true;
				move_right = true;
				force.Set(move_v, 0.0f);
			}
			else if (controller->left_stick_x_axis() < -moveCutOff)//left
			{
				move = true;
				move_right = false;
				force.Set(-move_v, 0.0f);
			}
			else
			{
				move = false;
				force.SetZero();
			//	body_->SetLinearVelocity(b2Vec2(0,0));
			}
		}
		else if (gDir == RIGHT || gDir == LEFT)
		{
			if (controller->left_stick_y_axis() > moveCutOff) // down
			{
				move = true;
				moveUp = false;
				force.Set(0.0f, -move_v);
			}
			else if (controller->left_stick_y_axis() < -moveCutOff) // up
			{
				move = true;
				moveUp = true;
				force.Set(0.0f, move_v);
			}
			else
			{
				move = false;
				force.SetZero();
				//body_->SetLinearVelocity(b2Vec2(0,0));
			}
		}

///////John///////////////////
		//attack
		if (controller->buttons_pressed() & ABFW_SONY_CTRL_L2 && attacking == false)
		{
			attacking = true;
		}
	}
	body_->ApplyForceToCenter(force);
}

////Rebecca
void Player::setGravity(int n)
{
	switch (n)
	{
		case 0:
			gDir = DOWN;
			gravity = b2Vec2(0.0f, -10.0f);
			break;
		case 1:
			gDir = RIGHT;
			gravity = b2Vec2(10.0f, 0.0f);
			break;
		case 2:
			gDir = UP;
			gravity = b2Vec2(0.0f, 10.0f);
			break;
		case 3:
			gDir = LEFT;
			gravity = b2Vec2(-10.0f, 0.0f);
			break;
	}
}

//change state
void Player::changeState()
{
	if(move == true && dead == false && state_ != INAIR)//only run when on a surface
	{
		state_ = RUNNING;
	}
	else if (attacking == true && state_ != INAIR)//no mid air attack
	{
		state_ = ATTACKING;
	}
	else if(dead == true)
	{
		state_ = DEAD;
		body_->SetLinearVelocity(b2Vec2(0,0));//stop all movement
	}
	else if (state_ != INAIR && state_ != JUMPING && attacking != true)//state stays as INAIR until a surface is touched
	{
		state_ = IDLE;
	}
}

void Player::runningAnimation()
{
	//flip for gravity
	if (gDir == UP)
	{
		set_uv_height(-0.25f);
	}
	else if (gDir == DOWN)
	{
		set_uv_height(0.25f);
	}
	else if (gDir == RIGHT)
	{
		set_uv_width(0.25f);
	}
	else if (gDir == LEFT)
	{
		set_uv_width(-0.25f);
	}
	
	if(horizontal == true)//horizontal animation
	{
		if(gDir == DOWN)
		{
			if(move_right == true)
			{
				set_uv_width(0.0625);
				set_uv_position(abfw::Vector2(0.0f,0.5f));
			}

			if(move_right == false)
			{
				set_uv_width(-0.0625);
				set_uv_position(abfw::Vector2(0.0625f,0.5f));
			}
		}
		else if(gDir == UP)
		{
			if(move_right == true)
			{
				set_uv_width(0.0625);
				set_uv_position(abfw::Vector2(0.0f,0.75f));
			}

			if(move_right == false)
			{
				set_uv_width(-0.0625);
				set_uv_position(abfw::Vector2(0.0625f,0.75f));
			}
		}

				
		//set up animation
		InitSpriteAnimation(0.0125,16,true,SCROLL_X,16,2);
	}
	else if (horizontal != true)//vertical animation
	{
		if(gDir == LEFT)
		{
			if(moveUp == true)
			{
				set_uv_height(0.0625);	
				set_uv_position(abfw::Vector2(0.75f,0.9375f));
			}

			if(moveUp == false)
			{
				set_uv_height(-0.0625);	
				set_uv_position(abfw::Vector2(0.75f,0.9375f));
			}
		}
		else if(gDir == RIGHT)
		{
			if(moveUp == true)
			{
				set_uv_height(0.0625);		
				set_uv_position(abfw::Vector2(0.5f,0.9375f));
			}

			if(moveUp == false)
			{
				set_uv_height(-0.0625);	
				set_uv_position(abfw::Vector2(0.5f,0.9375f));
			}
		}
				
		//set up animation
		InitSpriteAnimation(0.0125,16,true,SCROLL_Y,16,2);
	}	
}

void Player::idleAnimation()
{

	//flip for gravity
	if (gDir == UP)
	{
		set_uv_height(-0.5f);
	}
	else if (gDir == DOWN)
	{
		set_uv_height(0.5f);
	}
	else if (gDir == RIGHT)
	{
		set_uv_width(0.5f);
	}
	else if (gDir == LEFT)
	{
		set_uv_width(-0.5f);
	}

	//orientate sprite with direction
	if (horizontal == true)
	{
		if(gDir == DOWN)
		{
			if (move_right == true)
			{
				set_uv_width(0.03125f);
				set_uv_position(abfw::Vector2 (0.0f,0.0f));
			}
			else
			{
				set_uv_width(-0.03125f);
				set_uv_position(abfw::Vector2 (0.03125f,0.0f));
			}
		}
		else if (gDir == UP)
		{
			if (move_right == true)
			{
				set_uv_width(0.03125f);
				set_uv_position(abfw::Vector2 (0.0f,0.5f));
			}
			else
			{
				set_uv_width(-0.03125f);
				set_uv_position(abfw::Vector2 (0.03125f,0.5f));
			}
		}

		//set up animation
		InitSpriteAnimation(0.02,64,true,SCROLL_XY,32,2);
				
	}
	else
	{
		if(gDir == LEFT)
		{
			if (moveUp == true)
			{
				set_uv_height(0.03125);
				set_uv_position(abfw::Vector2(0.0f,0.96875f));
			}
			else
			{
				set_uv_height(-0.03125f);
				set_uv_position(abfw::Vector2(0.0f,1.0f));
			}
		}
		else if (gDir == RIGHT)
		{
			if (moveUp == true)
			{
				set_uv_height(0.03125f);
				set_uv_position(abfw::Vector2(0.0f,0.96875f));
			}
			else
			{
				set_uv_height(-0.03125f);
				set_uv_position(abfw::Vector2(0.0f,1.0f));
			}
		}

		//set up animation
		InitSpriteAnimation(0.02,64,true,SCROLL_YX,2,32);
	}
}

void Player::deadAnimation()
{

	//flip for gravity
	if (gDir == UP)
	{
		set_uv_height(-1);
	}
	else if (gDir == DOWN)
	{
		set_uv_height(1);
	}
	else if (gDir == RIGHT)
	{
		set_uv_width(1);
	}
	else if (gDir == LEFT)
	{
		set_uv_width(-1);
	}
		
	//orientate sprite with direction
	if (horizontal == true)
	{
		if(gDir == DOWN)
		{
			if (move_right == true)
			{
				set_uv_width(0.0625);
				set_uv_position(abfw::Vector2 (0.0f,0.0f));
			}
			else
			{
				set_uv_width(-0.0625);
				set_uv_position(abfw::Vector2 (0.0625,0.0f));
			}
		}
		else if (gDir == UP)
		{
			if (move_right == true)
			{
				set_uv_width(0.0625);
				set_uv_position(abfw::Vector2 (0.0f,1));
			}
			else
			{
				set_uv_width(-0.0625);
				set_uv_position(abfw::Vector2 (0.0625,1));
			}
		}				

		//set up animation
		InitSpriteAnimation(0.07,16,false,SCROLL_X,0,0);
				
	}
	else
	{
		if(gDir == LEFT)
		{
			if (moveUp == true)
			{
				set_uv_height(0.0625);
				set_uv_position(abfw::Vector2(1.0f,1.0f));
			}
			else
			{
				set_uv_height(-0.0625);
				set_uv_position(abfw::Vector2(1.0f,1.0f));
			}
		}
		else if (gDir == RIGHT)
		{
			if (moveUp == true)
			{
				set_uv_height(0.0625);
				set_uv_position(abfw::Vector2(0.0f,0.9375f));
			}
			else
			{
				set_uv_height(-0.0625);
				set_uv_position(abfw::Vector2(0.0f,1.0f));			}
		}

		//set up animation
		InitSpriteAnimation(0.07,16,false,SCROLL_Y,0,0);
	}
}

void Player::jumpAnimation(float xAxis, float yAxis)
{

	//flip for gravity
	if (gDir == UP)
	{
		set_uv_height(-1);
	}
	else if (gDir == DOWN)
	{
		set_uv_height(1);
	}
	else if (gDir == RIGHT)
	{
		set_uv_width(1);
	}
	else if (gDir == LEFT)
	{
		set_uv_width(-1);
	}
		
	//orientate sprite with direction
	if (horizontal == true)
	{
		//needs to be set from stick 	
		if(xAxis > 0)
		{
			move_right = true;
		}
		else if(xAxis < 0)
		{
			move_right = false;
		}

		if(gDir == DOWN)
		{
			if (move_right == true)
			{
				set_uv_width(0.0625);
				set_uv_position(abfw::Vector2 (0.0f,0.0f));
			}
			else
			{
				set_uv_width(-0.0625);
				set_uv_position(abfw::Vector2 (0.0625,0.0f));
			}
		}
		else if (gDir == UP)
		{
			if (move_right == true)
			{
				set_uv_width(0.0625);
				set_uv_position(abfw::Vector2 (0.0f,1));
			}
			else
			{
				set_uv_width(-0.0625);
				set_uv_position(abfw::Vector2 (0.0625,1));
			}
		}

		//set up animation
		InitSpriteAnimation(0.005,16,false,SCROLL_X,0,0);
				
	}
	else
	{
		//needs to be set from stick 	
		if(yAxis > 0)
		{
			moveUp = false;
		}
		else if(yAxis < 0)
		{
			moveUp = true;
		}

		if(gDir == LEFT)
		{
			if (moveUp == true)
			{
				set_uv_height(0.0625);
				set_uv_position(abfw::Vector2(0.0f,0.0625));
			}
			else
			{
				set_uv_height(-0.0625);
				set_uv_position(abfw::Vector2(0.0f,1.0f));
			}
		}
		else if (gDir == RIGHT)
		{
			if (moveUp == true)
			{
				set_uv_height(0.0625);
				set_uv_position(abfw::Vector2(0.0f,0.0625));
			}
			else
			{
				set_uv_height(-0.0625);
				set_uv_position(abfw::Vector2(0.0f,1.0f));
			}
		}

		//set up animation
		InitSpriteAnimation(0.005,16,false,SCROLL_Y,0,0);
	}
}

void Player::attackAnimation()
{
	//attack sprite is bigger so must be resized to match other sprites
	set_width(BOX2D_GFX_SIZE(2.5 * body_half_width));
	set_height(BOX2D_GFX_SIZE(2.15 * body_half_height));
	
	if(mflying == false)
	{
		//flip for gravity
		if (gDir == UP)
		{
			set_uv_height(-1);
		}
		else if (gDir == DOWN)
		{
			set_uv_height(1);
		}
		else if (gDir == RIGHT)
		{
			set_uv_width(1);
		}
		else if (gDir == LEFT)
		{
			set_uv_width(-1);
		}
	}
		
	//orientate sprite with direction

	if (horizontal == true)
	{
		if(gDir == DOWN)
		{
			if (move_right == true)
			{
				set_uv_width(0.0833);
				set_uv_position(abfw::Vector2 (0.0f,0.0f));
			}
			else
			{
				set_uv_width(-0.0833);
				set_uv_position(abfw::Vector2 (0.0833,0.0f));
			}
		}
		else if (gDir == UP)
		{
			if (move_right == true)
			{
				set_uv_width(0.0833);
				set_uv_position(abfw::Vector2 (0.0f,1));
			}
			else
			{
				set_uv_width(-0.0833);
				set_uv_position(abfw::Vector2 (0.0833,1));				
			}
		}				

		//set up animation
		InitSpriteAnimation(0.07,12,false,SCROLL_X,0,0);
				
	}
	else
	{
		if(gDir == LEFT)
		{
			if (moveUp == true)
			{
				set_uv_height(0.0833);
				set_uv_position(abfw::Vector2(1.0f,0.9277f));
			}
			else
			{
				set_uv_height(-0.0833);
				set_uv_position(abfw::Vector2(1.0f,1.0f));
			}
		}
		else if (gDir == RIGHT)
		{
			if (moveUp == true)
			{
				set_uv_height(0.0833);
				set_uv_position(abfw::Vector2(0.0f,0.9277f));
			}
			else
			{
				set_uv_height(-0.0833);
				set_uv_position(abfw::Vector2(0.0f,1.0f));
			}
		}

		//set up animation
		InitSpriteAnimation(0.07,12,false,SCROLL_Y,0,0);
	}
}

void Player::flyAnimation()
{

	if (move_right == true)
	{
		set_uv_width(0.0625f);
		set_uv_position(abfw::Vector2 (0.0f,0.0f));
	}
	else
	{
		set_uv_width(-0.0625f);
		set_uv_position(abfw::Vector2 (0.0625f,0.0f));
	}

	//set up animation
	InitSpriteAnimation(0.0125,16,true,SCROLL_X,16,2);
}
