#pragma once
#include "living_object.h"
class Player :
	public LivingObject
{

public:
	Player();
	virtual ~Player();

private:

	b2Vec2 gravity_;
	float jump_force_;
	float jump_recharge_time_;

	float jump_stick_cut_off_, moveStickCutOff_;

};

