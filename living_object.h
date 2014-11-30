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
	void UpdateHealth(const float deltahealth);
	inline void set_health(const float health) { health_ = health; }
	inline const float health() const { return health_; }
	inline void set_max_health(const float max_health) { max_health_ = max_health; }
	inline const float max_health() const { return max_health_; }
	
	void UpdateHealthPercent();
	inline const float health_percent() const { return health_percent_; }

	void DrawHealthBar(abfw::SpriteRenderer* sprite_renderer_, abfw::Font& font_); // Draws a thin health bar and health text above the parameter object

protected:
	float health_;
	float max_health_;
	float health_percent_;
	static const float kMaxHealthPercent = 100.0f;
};

#endif // _LIVING_OBJECT_H