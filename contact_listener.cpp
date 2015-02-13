#include "Contact_listener.h"
#include "game_object.h"
#include "player.h"
#include "platforms.h"
#include "enemy.h"
#include "collectible.h"
#include "plant.h"
#include "blade.h"

void Contact_Listener::BeginContact(b2Contact* contact)
{
	// get the colliding bodies
	b2Body* bodyA = contact->GetFixtureA()->GetBody();
	b2Body* bodyB = contact->GetFixtureB()->GetBody();

	// DO COLLISION RESPONSE HERE
	GameObject* game_object = (GameObject*)bodyA->GetUserData();
	GameObject* game_object_b = (GameObject*)bodyB->GetUserData();
		   
	if(game_object && game_object_b)//check user data is not null
	{
		//health decrease 
	/*	if (game_object->type_class == GameObject::PLAYER && game_object_b->type_class == GameObject::ENEMY && ((Player *)game_object)->attacking == false)
		{
			((Player *)game_object)->hurting = true;
			((Player *)game_object)->Knockback(BOX2D_GFX_POS_X(((Player *)game_object)->body->GetPosition().x),BOX2D_GFX_POS_X(((Enemy *)game_object_b)->body->GetPosition().x));
		}
		else if (game_object->type_class == GameObject::ENEMY && game_object_b->type_class == GameObject::PLAYER&& ((Player *)game_object_b)->attacking == false)
		{
			((Player *)game_object_b)->hurting = true;
			((Player *)game_object_b)->Knockback(BOX2D_GFX_POS_X(((Player *)game_object_b)->body->GetPosition().x), BOX2D_GFX_POS_X(((Enemy *)game_object)->body->GetPosition().x));
		}

		//attack enemies
		if (game_object->type_class == GameObject::PLAYER && game_object_b->type_class == GameObject::ENEMY && ((Player *)game_object)->attacking == true)
		{
			((Enemy* )game_object_b)->dead = true;
		}
		else if (game_object->type_class == GameObject::ENEMY && game_object_b->type_class == GameObject::PLAYER&& ((Player *)game_object_b)->attacking == true)
		{
			((Enemy* )game_object)->dead = true;
		}*/

		//pickup points
		if (game_object->type_class == GameObject::PLAYER && game_object_b->type_class == GameObject::PICKUP)
		{
			((PickUp *)game_object_b)->dead = true;
		}

		if (game_object->type_class == GameObject::PICKUP && game_object_b->type_class == GameObject::PLAYER)
		{
			((PickUp *)game_object)->dead = true;
		}

		//plant push you back
	/*	if (game_object->type_class == GameObject::PLAYER && game_object_b->type_class == GameObject::PLANT && ((Player *)game_object)->attacking == false)
		{
			((Player *)game_object)->Knockback(BOX2D_GFX_POS_X(((Player *)game_object)->body->GetPosition().x),BOX2D_GFX_POS_X(((Plant *)game_object_b)->body->GetPosition().x));
		}
		else if (game_object->type_class == GameObject::PLANT && game_object_b->type_class == GameObject::PLAYER && ((Player *)game_object_b)->attacking == false)
		{
			((Player *)game_object_b)->Knockback(BOX2D_GFX_POS_X(((Player *)game_object_b)->body->GetPosition().x), BOX2D_GFX_POS_X(((Plant *)game_object)->body->GetPosition().x));
		}

		if (game_object->type_class == GameObject::PLAYER && game_object_b->type_class == GameObject::PLANT && ((Player *)game_object)->attacking == true)
		{
			((Plant* )game_object_b)->dead = true;
		}
		else if (game_object->type_class == GameObject::PLANT && game_object_b->type_class == GameObject::PLAYER&& ((Player *)game_object_b)->attacking == true)
		{
			((Plant* )game_object)->dead = true;
		}*/

		//kill plants
		if (game_object->type_class == GameObject::BLADE && game_object_b->type_class == GameObject::PLANT)
		{
			((Plant* )game_object_b)->dead = true;
		}
		else if (game_object->type_class == GameObject::PLANT && game_object_b->type_class == GameObject::BLADE)
		{
			((Plant* )game_object)->dead = true;
		}

		//player death on spikes
		if (game_object->type_class == GameObject::PLAYER && game_object_b->type_class == GameObject::SPIKE)
		{
			((Player* )game_object)->dead = true;
		}
		else if (game_object->type_class == GameObject::SPIKE && game_object_b->type_class == GameObject::PLAYER)
		{
			((Player* )game_object_b)->dead = true;
		}

		//check player is touching platform
		if (game_object->type_class == GameObject::PLAYER && game_object_b->type_class == GameObject::PLATFORM)
		{
			((Player *)game_object)->grounded = true;
		}
		else if (game_object->type_class == GameObject::PLATFORM && game_object_b->type_class == GameObject::PLAYER)
		{
			((Player *)game_object_b)->grounded = true;
		}

		//if blade touches ground delete so player doesn't land on it
		if (game_object->type_class == GameObject::BLADE && game_object_b->type_class == GameObject::PLATFORM)
		{
			((Blade *)game_object)->dead = true;
		}
		else if (game_object->type_class == GameObject::PLATFORM && game_object_b->type_class == GameObject::BLADE)
		{
			((Blade *)game_object_b)->dead = true;
		}
	}
}

void Contact_Listener::EndContact(b2Contact* contact)
{
	// get the colliding bodies
	b2Body* bodyA = contact->GetFixtureA()->GetBody();
	b2Body* bodyB = contact->GetFixtureB()->GetBody();

	// DO COLLISION RESPONSE HERE
	GameObject* game_object = (GameObject*)bodyA->GetUserData();
	GameObject* game_object_b = (GameObject*)bodyB->GetUserData();
		   
	if(game_object && game_object_b)//check user data is not null
	{
		//health decrease 
		if (game_object->type_class == GameObject::PLAYER && game_object_b->type_class == GameObject::ENEMY)
		{
			((Player *)game_object)->hurting = false;
		}
		else if (game_object->type_class == GameObject::ENEMY && game_object_b->type_class == GameObject::PLAYER)
		{
			((Player *)game_object_b)->hurting = false;
		}

		//touching platform
		if (game_object->type_class == GameObject::PLAYER && game_object_b->type_class == GameObject::PLATFORM)
		{
			((Player *)game_object)->grounded = false;
		}
		else if (game_object->type_class == GameObject::PLATFORM && game_object_b->type_class == GameObject::PLAYER)
		{
			((Player *)game_object_b)->grounded = false;
		}
	}
}

