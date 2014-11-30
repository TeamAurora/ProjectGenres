#include "intro_state.h"
#include <graphics/sprite_renderer.h>
#include <audio/vita/audio_manager_vita.h>
#include <system/platform.h>
#include <graphics/texture.h>
#include "game_application.h"


IntroState::IntroState(abfw::Platform& platform, const GameApplication* application, abfw::AudioManager* audio_manager) :
	AppState(platform, application, audio_manager)
{
}


IntroState::~IntroState()
{
}

void IntroState::InitializeState()
{
	LoadTextures();

	splash_.InitSprite(platform_.width(), platform_.height(), abfw::Vector3(platform_.width()/2.0f, platform_.height()/2.0f, 0.0f), splash_texture_);
}
void IntroState::TerminateState()
{
	delete splash_texture_;
	splash_texture_ = NULL;
}
GAMESTATE IntroState::Update(const float& ticks_, const int& frame_counter_, const abfw::SonyControllerInputManager& controller_manager_)
{
	const abfw::SonyController* controller = controller_manager_.GetController(0); // get the platform specific controller from controller_manager
	if(controller) // if controller isn't null
	{
		if(controller->buttons_down() & ABFW_SONY_CTRL_START)
		{
			return MENU;
		}
	}
	return INTRO;
}
void IntroState::Render(const float frame_rate_, abfw::Font& font_, abfw::SpriteRenderer* sprite_renderer_)
{
	sprite_renderer_->DrawSprite(splash_);
}

void IntroState::LoadTextures()
{
	splash_texture_ = application_->LoadTextureFromPNG("Splash Screen.png");
}