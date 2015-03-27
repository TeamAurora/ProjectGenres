#include "Button.h"
#include <cstddef> // For NULL definition


Button::Button(abfw::Texture* default_texture, abfw::Texture* highlighted_texture) :
selected_(false)
{
	textures[0] = default_texture;
	textures[1] = highlighted_texture;
	set_texture(textures[0]);
}


Button::~Button()
{
	DeleteNull(textures[0]);
	DeleteNull(textures[1]);
}

void Button::Select(bool state)
{
	selected_ = state;
	switch (selected_)
	{
	case true:
		set_texture(textures[0]);
		break;
	case false:
		set_texture(textures[1]);
		break;
	}
}