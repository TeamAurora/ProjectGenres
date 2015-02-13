#ifndef _LIVING_OBJECT_H
#define _LIVING_OBJECT_H

#include "game_object.h"

namespace abfw
{
	class SpriteRenderer;
	class Font;
}

class LivingObject : public GameObject
{ // Game object considered to be a living entity(has health, not necessarily alive e.g. machine or undead still derives from this class)
public:
	LivingObject();
	virtual ~LivingObject();
	
	// update health by deltahealth parameter (doesn't exceed maxhealth)
	void updateHealth(const float deltahealth);
	inline void setHealth(const float health) { health_ = health; }
	inline const float health() const { return health_; }
	inline void setMaxHealth(const float max_health) { max_health_ = max_health; }
	inline const float max_health() const { return max_health_; }
	
	void updateHealthPercent();
	inline const float health_percent() const { return health_percent_; }

	void drawHealthBar(abfw::SpriteRenderer* sprite_renderer_, abfw::Font& font_); // Draws a thin health bar and health text above the parameter object

private:
	float health_;
	float max_health_;
	float health_percent_;
};

#endif // _LIVING_OBJECT_H