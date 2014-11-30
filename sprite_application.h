#ifndef _SPRITE_APPLICATION_H
#define _SPRITE_APPLICATION_H

#include <system/application.h>
#include "game_object.h"
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <input/sony_controller_input_manager.h>

namespace abfw
{
	class Platform;
}

class SpriteApplication : public abfw::Application
{
public:
	SpriteApplication(abfw::Platform& platform);
	~SpriteApplication();
	void Init();
	void CleanUp();
	bool Update(float ticks);
	void Render();

private:
	abfw::SonyControllerInputManager* controller_manager_;
	GameObject game_object_;
	abfw::Font font_;
	abfw::SpriteRenderer* sprite_renderer_;
	float frame_rate_;
};

#endif // _SPRITE_APPLICATION_H