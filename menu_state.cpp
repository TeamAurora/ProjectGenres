#include "menu_state.h"
#include <graphics/sprite_renderer.h>
#include <audio/vita/audio_manager_vita.h>
#include <system/platform.h>
#include <graphics/texture.h>
#include "game_application.h"
#include <sstream>


MenuState::MenuState(abfw::Platform& platform, const GameApplication* application, abfw::AudioManager* audio_manager) :
	AppState(platform, application, audio_manager),
	selection_(START),
	render_(MAIN_MENU)
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

	start_button_ = Button(application_->LoadTextureFromPNG("start_button.png"), application_->LoadTextureFromPNG("start_button_highlighted.png"));
	start_button_.set_position(abfw::Vector3(750.0f, 190.0f, 0.0f));

	help_button_ = Button(application_->LoadTextureFromPNG("help_button.png"), application_->LoadTextureFromPNG("help_button_highlighted.png"));
	help_button_.set_position(abfw::Vector3(783.0f, 290.0f, 0.0f));

	options_button_ = Button(application_->LoadTextureFromPNG("options_button.png"), application_->LoadTextureFromPNG("options_button_highlighted.png"));
	options_button_.set_position(abfw::Vector3(717.0f, 400.0f, 0.0f));

	for (int i = 0; i < 3; i++)
	{
		std::stringstream default_texture, highlighted_texture;
		default_texture << "level_" << i << "_button.png";
		highlighted_texture << "level_" << i << "_button_highlighted.png";
		level_buttons[i] = Button(application_->LoadTextureFromPNG(default_texture.str().c_str()), application_->LoadTextureFromPNG(highlighted_texture.str().c_str()));
	}
}

void MenuState::TerminateState()
{
	DeleteNull(main_menu_background_texture_);
	DeleteNull(help_screen_background_texture_);
	DeleteNull(options_screen_background_texture_);
	DeleteNull(level_select_background_texture_);
}

APPSTATE MenuState::Update(const float& ticks_, const int& frame_counter_, const abfw::SonyControllerInputManager& controller_manager_)
{
	const abfw::SonyController* controller = controller_manager_.GetController(0); // get the platform specific controller from controller_manager
	if (controller) // if controller isn't null
	{
		switch (render_)
		{
		case MAIN_MENU:
			switch (selection_)
			{
			case START:
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_CROSS)
				{
					render_ = LEVEL_SELECT;
					background_.set_texture(level_select_background_texture_);
				}
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_DOWN)
				{
					selection_ = HELP;
					help_button_.Select(true);
					start_button_.Select(false);
				}
				break;
			case HELP:
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_UP)
				{
					selection_ = START;
					start_button_.Select(true);
					help_button_.Select(false);
				}
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_CROSS)
				{
					render_ = HELP_SCREEN;
					background_.set_texture(help_screen_background_texture_);
				}
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_DOWN)
				{
					selection_ = OPTIONS;
					options_button_.Select(true);
					help_button_.Select(false);
				}
				break;
			case OPTIONS:
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_UP)
				{
					selection_ = HELP;
					help_button_.Select(true);
					options_button_.Select(false);
				}
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_CROSS)
				{
					render_ = OPTIONS_SCREEN;
					background_.set_texture(options_screen_background_texture_);
				}
				break;
			}
			break;
		case HELP_SCREEN:
			if (controller->buttons_pressed() & ABFW_SONY_CTRL_CIRCLE)
			{
				render_ = MAIN_MENU;
				background_.set_texture(main_menu_background_texture_);
			}
			break;
		case OPTIONS_SCREEN:
			if (controller->buttons_pressed() & ABFW_SONY_CTRL_CIRCLE)
			{
				render_ = MAIN_MENU;
				background_.set_texture(main_menu_background_texture_);
			}
			break;
		case LEVEL_SELECT:
			if (level_selection_ > 1)
			{
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_LEFT)
				{
					level_buttons[level_selection_].Select(false);
					level_selection_--;
					level_buttons[level_selection_].Select(true);
				}
			}

			if (level_selection_ < 3)
			{
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_RIGHT)
				{
					level_buttons[level_selection_].Select(true);
					level_selection_++;
					level_buttons[level_selection_].Select(false);
				}
			}

			if (controller->buttons_pressed() & ABFW_SONY_CTRL_CROSS)
			{
				switch (level_selection_)
				{
				case 1:
					return LEVEL_1;
					break;
				case 2:
					return LEVEL_2;
					break;
				case 3:
					return LEVEL_3;
					break;
				}
			}

			if (controller->buttons_pressed() & ABFW_SONY_CTRL_CIRCLE)
			{
				render_ = MAIN_MENU;
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
	switch (render_)
	{
	case MAIN_MENU:
		sprite_renderer_->DrawSprite(start_button_);
		sprite_renderer_->DrawSprite(help_button_);
		sprite_renderer_->DrawSprite(options_button_);
		break;
	case LEVEL_SELECT:
		for (int i = 0; i < 3; i++)
		{
			sprite_renderer_->DrawSprite(level_buttons[i]);
		}
		break;
	}
}

void MenuState::LoadTextures()
{
	main_menu_background_texture_ = application_->LoadTextureFromPNG("main_menu_background.png");
	help_screen_background_texture_ = application_->LoadTextureFromPNG("help_menu_background.png");
	options_screen_background_texture_ = application_->LoadTextureFromPNG("options_menu_background.png");
	level_select_background_texture_ = application_->LoadTextureFromPNG("level_select_background.png");
}