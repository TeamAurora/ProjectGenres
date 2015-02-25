#include "Player.h"
#include <input/sony_controller_input_manager.h>
#include "box2d_helpers.h"


Player::Player()
{
	// set gameobject type
	type_ = PLAYER;

	// start idle
	state_ = IDLE;

	//set values for variables
	move_v = 6;
	jump_v = 160.0f;
	jump_gap = 35;
	damage = 2.5;
	knockbackForce.Set(150, 0.0f);

	//texture coords
	uv_x = 0.0f;
	uv_y = 0.0f;
	uv_width = -1.0f;
	uv_height = 1.0f;

	//player states
	attacking = false;
	hurting = false;
	move_right = false;
	horizontal = true;
	moveUp = true;
	grounded = true;
	jumping = false;

	jumpCutOff = 0.8;
	moveCutOff = 0.6;
	currentRayAngle = 0; //5.235987756 ;//300 radians
	rayLength = 1;
	Player_Gravity(0);
}


Player::~Player()
{
}

void Player::Create_Player(b2World* world_, float width, float height)
{
	//varaibles that need reset if player has been killed
	dead = false;
	destroyed = false;
	setMaxHealth(100.0f);
	setHealth(max_health());

	// sets initial gravity to be down
	gravity = b2Vec2(0.0f, -10.0f);
	gDir = DOWN;

	//create body
	bodyInitialPosition.x = GFX_BOX2D_POS_X(width*0.85);
	bodyInitialPosition.y = GFX_BOX2D_POS_Y(height*0.85);

	body_half_width = 0.35f;
	body_half_height = 0.35f;

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.x = bodyInitialPosition.x;
	bodyDef.position.y = bodyInitialPosition.y;
	bodyDef.fixedRotation = true;//stops body rotating 
	body_ = world_->CreateBody(&bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(body_half_width, body_half_height);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 2.0f;//set mass
	body_->ResetMassData();//it only sets new value after this is called
	fixtureDef.friction = 0.5f;
	fixtureDef.restitution = 0.1f; // not bouncy
	body_->CreateFixture(&fixtureDef);

	body_->SetUserData(this);

	//set sprite size to match body
	set_width(BOX2D_GFX_SIZE(2 * body_half_width));
	set_height(BOX2D_GFX_SIZE(2 * body_half_height));

	//uv height, width and position
	set_uv_height(uv_height);
	set_uv_width(uv_width);
	set_uv_position(abfw::Vector2(uv_x, uv_y));
}

void Player::Update(const float& ticks, bool gameOver)
{
	//update sprite position to match body
	float new_x = BOX2D_GFX_POS_X(body_->GetPosition().x);
	float new_y = BOX2D_GFX_POS_Y(body_->GetPosition().y);

	set_position(abfw::Vector3(new_x, new_y, 0.f));
	set_rotation(-body_->GetAngle());

	currentPos = b2Vec2(new_x, new_y);

	if(hurting)
	{
		updateHealth(-damage);
	}

	if(health() <= 0)
	{
		dead = true;
	}

	if(gDir == UP || gDir == DOWN)
	{
		horizontal = true;
	}
	else
	{
		horizontal = false;
	}

	//orientate sprite with direction
	if (horizontal == true)
	{
		if (move_right == true)
		{
			set_uv_width(1.0);
		}
		else
		{
			set_uv_width(-1.0);
		}
	}
	else
	{
		if (moveUp == true)
		{
			set_uv_height(-1.0);
		}
		else
		{
			set_uv_height(1.0);
		}
	}

	//flip for gravity
	if (gDir == UP)
	{
		set_uv_height(-1.0);
	}
	else if (gDir == DOWN)
	{
		set_uv_height(1.0);
	}
	else if (gDir == RIGHT)
	{
		set_uv_width(-1.0f);
	}
	else if (gDir == LEFT)
	{
		set_uv_width(1.0f);
	}

	//apply gravity
	body_->ApplyForceToCenter(gravity);
}

void Player::Player_Input(const abfw::SonyController* controller)
{

	// check we have a valid controller object (one that isn't NULL)
	if (controller)
	{
		/*// Sets current gravity manipulation via player input
		if (controller->right_stick_y_axis() < -jumpCutOff)//up
		{
			gDir = UP;
			gravity = b2Vec2(0.0f, 10.0f);
		}
		else if (controller->right_stick_x_axis() > jumpCutOff)//right
		{
			gDir = RIGHT;
			gravity = b2Vec2(10.0f, 0.0f);
		}
		else if (controller->right_stick_x_axis() < -jumpCutOff)//left
		{
			gDir = LEFT;
			gravity = b2Vec2(-10.0f, 0.0f);
		}
		else if (controller->right_stick_y_axis()> jumpCutOff)//down
		{
			gDir = DOWN;
			gravity = b2Vec2(0.0f, -10.0f);
		}*/

		// checks what gravity is currently applied and dictates what movement
		// can be used ie. x axis movement on ground/ceiling, y axis movement on
		// walls
		if (gDir == DOWN || gDir == UP)
		{
			if (controller->left_stick_x_axis() > moveCutOff)//right
			{
				move_right = true;
				force.Set(move_v, 0.0f);
			}
			else if (controller->left_stick_x_axis() < -moveCutOff)//left
			{
				move_right = false;
				force.Set(-move_v, 0.0f);
			}
			else
			{
				force.SetZero();
			}
		}
		else if (gDir == RIGHT || gDir == LEFT)
		{
			if (controller->left_stick_y_axis() > moveCutOff) // down
			{
				moveUp = false;
				force.Set(0.0f, -move_v);
			}
			else if (controller->left_stick_y_axis() < -moveCutOff) // up
			{
				moveUp = true;
				force.Set(0.0f, move_v);
			}
			else
			{
				force.SetZero();
			}
		}

		//attack
		if (controller->buttons_down() & ABFW_SONY_CTRL_L2 && attacking == false)
		{
			attacking = true;
		}
		else
		{
			attacking = false;
		}

		
		// change angle of projected jump
		float xaxisval = controller->right_stick_x_axis();
		float yaxisval = controller->right_stick_y_axis();
		currentRayAngle = atan2(xaxisval, -yaxisval); //* FRAMEWORK_RAD_TO_DEG;
	

		// jump mechanic
		if ( controller->buttons_down() & ABFW_SONY_CTRL_R2)
		{
			b2Vec2 angle = b2Vec2(sinf(currentRayAngle), cosf(currentRayAngle) );

			b2Vec2 impulse = b2Vec2(rayLength * angle);

			jumping = true;
			body_->ApplyLinearImpulse(impulse, 
				body_->GetWorldCenter() ); // apply force to robot using p2 as direction
		}
		else
		{
			jumping = false;
		}
		
	}
	body_->ApplyForceToCenter(force);
}

void Player::Player_Gravity(int n)
{
	switch (n) {
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