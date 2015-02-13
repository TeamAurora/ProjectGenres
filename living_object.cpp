#include "living_object.h"
#include <graphics/font.h>
#include <graphics/sprite_renderer.h>

LivingObject::LivingObject() :
	health_(100.0f),
	max_health_(100.0f)
{
}

LivingObject::~LivingObject()
{
}

void LivingObject::updateHealth(const float deltahealth)
{ // updates health by adding deltahealth
	health_ += deltahealth;
	if (health_ > max_health_)
	{	// Health can never exceed maximum health
		health_ = max_health_;
	}
}

void LivingObject::updateHealthPercent()
{
	health_percent_ = (health_/max_health_) * 100;
}

void LivingObject::drawHealthBar(abfw::SpriteRenderer* sprite_renderer_, abfw::Font& font_)
{
	updateHealthPercent(); // calculates objects health percent
	abfw::Sprite health_bar;
	health_bar.set_width((width()*health_percent()/100)); // width % of object width = % of max health
	health_bar.set_height(6.0f);
	health_bar.set_position(abfw::Vector3(position().x, position().y - height() - 6.0f, 0.0f)); // sets health bar position to 6.0f above the object
	health_bar.set_colour(0xff0000ff); // red
	sprite_renderer_->DrawSprite(health_bar);
	font_.RenderText(sprite_renderer_, health_bar.position(), 0.4f, 0xff00ffff, abfw::TJ_CENTRE, "%.0f/%.0f", health(), max_health()); // draws health text
}