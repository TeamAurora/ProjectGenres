#pragma once
#include "sprite.h"
#include <input/vita/sony_controller_input_manager_vita.h>

class Button : public Sprite
{
public:
	Button(abfw::Texture* default_texture, abfw::Texture* highlighted_texture);
	~Button();

	void Select(bool state);
	bool Selected() const { return selected_; }

private:

	const abfw::Texture* textures[2];
	bool selected_;
};