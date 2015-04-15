#pragma once
#include "sprite.h"
#include <input/vita/sony_controller_input_manager_vita.h>

class Button : public Sprite
{
public:

	Button();
	Button(abfw::Texture* default_texture, abfw::Texture* highlighted_texture);
	virtual ~Button();

	void Select(bool state);

private:

	const abfw::Texture* textures[2];
	bool selected_;
};