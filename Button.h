#pragma once
#include "sprite.h"
#include <input/vita/sony_controller_input_manager_vita.h>

class Button : public Sprite
{
public:
	Button();
	~Button();

	void InputLoop(const abfw::SonyController* controller_);

	bool Selected;

	//void SetIndexedTexture(const abfw::Texture* const texture, int index) { textures_[index] = texture; }
	//const abfw::Texture* const IndexedTexture(int index) const { return textures_[index]; }

private:

	//virtual void DoInput(const abfw::SonyController* controller_) = 0;

	//const abfw::Texture* textures_[2];
};