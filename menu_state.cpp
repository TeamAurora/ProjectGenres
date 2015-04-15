#include "menu_state.h"
#include "game_application.h"
#include <graphics\sprite_renderer.h>
#include <graphics\texture.h>
#include <sstream>


MenuState::MenuState(abfw::Platform& platform, const GameApplication* application, abfw::AudioManager* audio_manager) :
	AppState(platform, application, audio_manager),
	main_menu_selection_(START),
	menustate_(MAIN_MENU),
	level_selection_(1),
	options_selection_(MUSIC),
	start_button_(application_->LoadTextureFromPNG("start_button.png"), application_->LoadTextureFromPNG("start_button_highlighted.png")),
	help_button_(application_->LoadTextureFromPNG("help_button.png"), application_->LoadTextureFromPNG("help_button_highlighted.png")),
	options_button_(application_->LoadTextureFromPNG("options_button.png"), application_->LoadTextureFromPNG("options_button_highlighted.png"))
{
	start_button_.set_position(abfw::Vector3(750.0f, 190.0f, 0.0f));
	help_button_.set_position(abfw::Vector3(783.0f, 290.0f, 0.0f));
	options_button_.set_position(abfw::Vector3(717.0f, 400.0f, 0.0f));

	options_buttons_[0] = Button(application_->LoadTextureFromPNG("music_button.png"), application_->LoadTextureFromPNG("music_button_highlighted.png"));
	options_buttons_[1] = Button(application_->LoadTextureFromPNG("sfx_button.png"), application_->LoadTextureFromPNG("sfx_button_highlighted.png"));

	for (int buttonindex = 0; buttonindex < options_buttons_.size(); buttonindex++)
	{
		options_buttons_[buttonindex].set_position(470.0f, (platform_.height() / options_buttons_.size() + 2) * buttonindex + 1, 0.0f);
	}

	for (int buttonindex = 0; buttonindex < level_buttons_.size(); buttonindex++)
	{
		std::stringstream default_texture, highlighted_texture;
		default_texture << "level_" << buttonindex << "_button.png";
		highlighted_texture << "level_" << buttonindex << "_button_highlighted.png";
		level_buttons_[buttonindex] = Button(application_->LoadTextureFromPNG(default_texture.str().c_str()), application_->LoadTextureFromPNG(highlighted_texture.str().c_str()));
		level_buttons_[buttonindex].set_position((platform_.width() / level_buttons_.size() + 2) * buttonindex + 1, (platform_.height() / 2.0f), 0.0f);
	}
}


MenuState::~MenuState()
{
}

void MenuState::InitializeState()
{
	LoadTextures();

	abfw::Vector3 screen_centre(platform_.width()/2.0f, platform_.height()/2.0f, 0.0f);
	
	background_ = Sprite();
	background_.InitSprite(platform_.width(), platform_.height(), screen_centre, menu_background_texture_);

	background_overlay_ = Sprite();
	background_overlay_.InitSprite(platform_.width(), platform_.height(), screen_centre, main_menu_texture_);
}

void MenuState::TerminateState()
{
	DeleteNull(menu_background_texture_);
	DeleteNull(main_menu_texture_);
	DeleteNull(help_screen_texture_);
	DeleteNull(options_screen_texture_);
	DeleteNull(level_select_texture_);
}

APPSTATE MenuState::Update(const float& ticks_, const int& frame_counter_, const abfw::SonyControllerInputManager& controller_manager_)
{
	const abfw::SonyController* controller = controller_manager_.GetController(0); // get the platform specific controller from controller_manager
	if (controller) // if controller isn't null
	{
		switch (menustate_)
		{
		case MAIN_MENU:
			switch (main_menu_selection_)
			{
			case START:
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_CROSS)
				{
					menustate_ = LEVEL_SELECT;
					background_overlay_.set_texture(level_select_texture_);
				}
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_DOWN)
				{
					main_menu_selection_ = HELP;
					help_button_.Select(true);
					start_button_.Select(false);
				}
				break;
			case HELP:
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_UP)
				{
					main_menu_selection_ = START;
					start_button_.Select(true);
					help_button_.Select(false);
				}
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_CROSS)
				{
					menustate_ = HELP_SCREEN;
					background_overlay_.set_texture(help_screen_texture_);
				}
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_DOWN)
				{
					main_menu_selection_ = OPTIONS;
					options_button_.Select(true);
					help_button_.Select(false);
				}
				break;
			case OPTIONS:
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_UP)
				{
					main_menu_selection_ = HELP;
					help_button_.Select(true);
					options_button_.Select(false);
				}
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_CROSS)
				{
					menustate_ = OPTIONS_SCREEN;
					background_overlay_.set_texture(options_screen_texture_);
				}
				break;
			}
			break;
		case HELP_SCREEN:
			if (controller->buttons_pressed() & ABFW_SONY_CTRL_CIRCLE)
			{
				menustate_ = MAIN_MENU;
				background_overlay_.set_texture(main_menu_texture_);
			}
			break;
		case OPTIONS_SCREEN:
			if (controller->buttons_pressed() & ABFW_SONY_CTRL_CIRCLE)
			{
				menustate_ = MAIN_MENU;
				background_overlay_.set_texture(main_menu_texture_);
			}
			switch (options_selection_)
			{
			case MUSIC:
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_CROSS)
				{
					switch (application_->settings_.music_)
					{
					case true:
						application_->settings_.music_ = false;
						break;
					case false:
						application_->settings_.music_ = true;
						break;
					}
				}
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_DOWN)
				{
					options_selection_ = SFX;
					options_buttons_[1].Select(true);
					options_buttons_[0].Select(false);
				}
				break;
			case SFX:
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_UP)
				{
					options_selection_ = MUSIC;
					options_buttons_[0].Select(true);
					options_buttons_[1].Select(false);
				}
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_CROSS)
				{
					switch (application_->settings_.sound_effects_)
					{
					case true:
						application_->settings_.sound_effects_ = false;
						break;
					case false:
						application_->settings_.sound_effects_ = true;
						break;
					}
				}
				break;
			}
			break;
		case LEVEL_SELECT:
			if (level_selection_ > 1)
			{
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_LEFT)
				{
					level_buttons_[level_selection_].Select(false);
					level_selection_--;
					level_buttons_[level_selection_].Select(true);
				}
			}

			if (level_selection_ < level_buttons_.size())
			{
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_RIGHT)
				{
					level_buttons_[level_selection_].Select(true);
					level_selection_++;
					level_buttons_[level_selection_].Select(false);
				}
			}

			if (controller->buttons_pressed() & ABFW_SONY_CTRL_CROSS)
			{
				switch (level_selection_)
				{
				case 1:
					return LEVEL_1;
				case 2:
					return LEVEL_2;
				case 3:
					return LEVEL_3;
				}
			}

			if (controller->buttons_pressed() & ABFW_SONY_CTRL_CIRCLE)
			{
				menustate_ = MAIN_MENU;
				background_overlay_.set_texture(main_menu_texture_);
			}
			break;
		}
	}
	return MENU;
}

void MenuState::Render(const float frame_rate_, abfw::Font& font_, abfw::SpriteRenderer* sprite_renderer_)
{
	sprite_renderer_->DrawSprite(background_);
	sprite_renderer_->DrawSprite(background_overlay_);
	switch (menustate_)
	{
	case MAIN_MENU:
		sprite_renderer_->DrawSprite(start_button_);
		sprite_renderer_->DrawSprite(help_button_);
		sprite_renderer_->DrawSprite(options_button_);
		break;
	case LEVEL_SELECT:
		for (int buttonindex = 0; buttonindex < level_buttons_.size(); buttonindex++)
		{
			sprite_renderer_->DrawSprite(level_buttons_[buttonindex]);
		}
		break;
	case OPTIONS:
		for (int buttonindex = 0; buttonindex < options_buttons_.size(); buttonindex++)
		{
			sprite_renderer_->DrawSprite(options_buttons_[buttonindex]);
		}
		break;
	}
}

void MenuState::LoadTextures()
{
	menu_background_texture_ = application_->LoadTextureFromPNG("menu_background.png");
	main_menu_texture_ = application_->LoadTextureFromPNG("main_menu_background.png");
	help_screen_texture_ = application_->LoadTextureFromPNG("help_menu_background.png");
	options_screen_texture_ = application_->LoadTextureFromPNG("options_menu_background.png");
	level_select_texture_ = application_->LoadTextureFromPNG("level_select_background.png");
}