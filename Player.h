#pragma once
#include "living_object.h"
class Player :
	public LivingObject
{

public:

	Player();
	virtual ~Player();

	// Example attack prototype
	void Attack(float damage) const;

	// to knockback player use gameobject Force function
	// to do state-based logic use state_ member from gameobject

private:

	b2Vec2 gravity_;
	float jump_force_;
	float jump_recharge_time_;
	float jump_stick_cut_off_, moveStickCutOff_;

};

