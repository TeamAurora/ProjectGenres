#include "menu_state.h"
#include <graphics/sprite_renderer.h>
#include <audio/vita/audio_manager_vita.h>
#include <system/platform.h>
#include <graphics/texture.h>
#include "game_application.h"


MenuState::MenuState(abfw::Platform& platform, const GameApplication* application, abfw::AudioManager* audio_manager) :
	AppState(platform, application, audio_manager),
	selection_(START),
	state_(MAIN_MENU),
	level_selection_(1)
{
}


MenuState::~MenuState()
{
}

void MenuState::InitializeState()
{
	LoadTextures();

	abfw::Vector3 screen_centre(platform_.width()/2.0f, platform_.height()/2.0f, 0.0f);
	
	background_ = Sprite();
	background_.InitSprite(platform_.width(), platform_.height(), screen_centre, main_menu_background_texture_);

	start_button_ = Button();
	start_button_.InitSprite(201.0f, 66.0f, abfw::Vector3(750.0f, 190.0f, 0.0f), start_button_highlighted_texture_);

	help_button_ = Button();
	help_button_.InitSprite(166.0f, 57.0f, abfw::Vector3(783.0f, 290.0f, 0.0f), help_button_texture_);

	options_button_ = Button();
	options_button_.InitSprite(246.0f, 57.0f, abfw::Vector3(717.0f, 400.0f, 0.0f), options_button_texture_);
}

void MenuState::TerminateState()
{
	DeleteNull(main_menu_background_texture_);
	DeleteNull(help_screen_background_texture_);
	DeleteNull(options_screen_background_texture_);
	DeleteNull(level_select_background_texture_);
	DeleteNull(start_button_texture_);
	DeleteNull(start_button_highlighted_texture_);
	DeleteNull(help_button_texture_);
	DeleteNull(help_button_highlighted_texture_);
	DeleteNull(options_button_texture_);
	DeleteNull(options_button_highlighted_texture_);
}

APPSTATE MenuState::Update(const float& ticks_, const int& frame_counter_, const abfw::SonyControllerInputManager& controller_manager_)
{
	const abfw::SonyController* controller = controller_manager_.GetController(0); // get the platform specific controller from controller_manager
	if (controller) // if controller isn't null
	{
		switch (state_)
		{
		case MAIN_MENU:
			switch (selection_)
			{
			case START:
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_CROSS)
				{
					state_ = LEVEL_SELECT;
					background_.set_texture(level_select_background_texture_);
				}
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_DOWN)
				{
					selection_ = HELP;
					help_button_.set_texture(help_button_highlighted_texture_);
					start_button_.set_texture(start_button_texture_);
				}
				break;
			case HELP:
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_UP)
				{
					selection_ = START;
					start_button_.set_texture(start_button_highlighted_texture_);
					help_button_.set_texture(help_button_texture_);
				}
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_CROSS)
				{
					state_ = HELP_SCREEN;
					background_.set_texture(help_screen_background_texture_);
				}
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_DOWN)
				{
					selection_ = OPTIONS;
					options_button_.set_texture(options_button_highlighted_texture_);
					help_button_.set_texture(help_button_texture_);
				}
				break;
			case OPTIONS:
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_UP)
				{
					selection_ = HELP;
					help_button_.set_texture(help_button_highlighted_texture_);
					options_button_.set_texture(options_button_texture_);
				}
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_CROSS)
				{
					state_ = OPTIONS_SCREEN;
					background_.set_texture(options_screen_background_texture_);
				}
				break;
			}
			break;
		case HELP_SCREEN:
			if (controller->buttons_pressed() & ABFW_SONY_CTRL_CIRCLE)
			{
				state_ = MAIN_MENU;
				background_.set_texture(main_menu_background_texture_);
			}
			break;
		case OPTIONS_SCREEN:
			if (controller->buttons_pressed() & ABFW_SONY_CTRL_CIRCLE)
			{
				state_ = MAIN_MENU;
				background_.set_texture(main_menu_background_texture_);
			}
			break;
		case LEVEL_SELECT:
			if (controller->buttons_pressed() & ABFW_SONY_CTRL_CROSS)
			{
				return LEVEL_1;
			}
			if (controller->buttons_pressed() & ABFW_SONY_CTRL_CIRCLE)
			{
				state_ = MAIN_MENU;
				background_.set_texture(main_menu_background_texture_);
			}
			break;
		}
	}
	return MENU;
}

void MenuState::Render(const float frame_rate_, abfw::Font& font_, abfw::SpriteRenderer* sprite_renderer_)
{
	sprite_renderer_->DrawSprite(background_);
	if (state_ == MAIN_MENU)
	{
		sprite_renderer_->DrawSprite(start_button_);

		sprite_renderer_->DrawSprite(help_button_);

		sprite_renderer_->DrawSprite(options_button_);
	}
}

void MenuState::LoadTextures()
{
	main_menu_background_texture_ = application_->LoadTextureFromPNG("main_menu_background.png");
	help_screen_background_texture_ = application_->LoadTextureFromPNG("help_menu_background.png");
	options_screen_background_texture_ = application_->LoadTextureFromPNG("options_menu_background.png");
	level_select_background_texture_ = application_->LoadTextureFromPNG("level_select_background.png");
	start_button_texture_ = application_->LoadTextureFromPNG("start_button.png");
	help_button_texture_ = application_->LoadTextureFromPNG("help_button.png");
	options_button_texture_ = application_->LoadTextureFromPNG("options_button.png");
	start_button_highlighted_texture_ = application_->LoadTextureFromPNG("start_button_highlighted.png");
	help_button_highlighted_texture_ = application_->LoadTextureFromPNG("help_button_highlighted.png");
	options_button_highlighted_texture_ = application_->LoadTextureFromPNG("options_button_highlighted.png");
}