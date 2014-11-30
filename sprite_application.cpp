#include "sprite_application.h"
#include <system/platform.h>
#include <iostream>

SpriteApplication::SpriteApplication(abfw::Platform& platform) :
	abfw::Application(platform),
	sprite_renderer_(NULL),
	controller_manager_(NULL)
{
}

SpriteApplication::~SpriteApplication()
{
}

void SpriteApplication::Init()
{
	// load the font to draw the on-screen text
	bool font_loaded = font_.Load("comic_sans", platform_);
	if(!font_loaded)
	{
		std::cout << "Font failed to load." << std::endl;
		exit(-1);
	}

	// create a sprite renderer to draw the sprites
	sprite_renderer_ = platform_.CreateSpriteRenderer();

	// create a controller manager to receive controller input
	controller_manager_ = platform_.CreateSonyControllerInputManager();

	//
	// initialise the game object properties
	//
	// set the width and height to be 32
	game_object_.set_width(32.0f);
	game_object_.set_height(32.0f);

	// set the sprite position to be the centre of the screen
	game_object_.set_position(abfw::Vector3(platform_.width()*0.5f, platform_.height()*0.5f, 0.f));
}

void SpriteApplication::CleanUp()
{
	// free up controller manager
	delete controller_manager_;
	controller_manager_ = NULL;

	// free up the sprite renderer
	delete sprite_renderer_;
	sprite_renderer_ = NULL;
}

bool SpriteApplication::Update(float ticks)
{
	// calculate the frame rate
	frame_rate_ = 1.0f / ticks;

	controller_manager_->Update();

	// get the first controller attached to the hardware platform
	// we only have one on PS Vita
	const abfw::SonyController* controller = controller_manager_->GetController(0);

	// check we have a valid controller object (one that isn't NULL)
	if(controller)
	{
		if ((controller->buttons_down() & (ABFW_SONY_CTRL_START | ABFW_SONY_CTRL_SELECT)) == (ABFW_SONY_CTRL_START | ABFW_SONY_CTRL_SELECT))
		{
			std::cout << "start and select pressed" << std::endl;
		}
	}


	// return if the application is still running
	// this should always be true on PS Vita
	return true;
}

void SpriteApplication::Render()
{
	// set up sprite renderer for drawing
	sprite_renderer_->Begin();

	//
	// draw all sprites here
	//
	sprite_renderer_->DrawSprite(game_object_);

	font_.RenderText(sprite_renderer_, abfw::Vector3(10.0f, 5.0f, -0.9f), 1.0f, 0xff0000ff, abfw::TJ_LEFT, "My First Sprite Application");
	font_.RenderText(sprite_renderer_, abfw::Vector3(850.0f, 510.0f, -0.9f), 1.0f, 0xff00ffff, abfw::TJ_LEFT, "FPS: %.1f", frame_rate_);

	// tell sprite renderer that all sprites have been drawn
	sprite_renderer_->End();
}