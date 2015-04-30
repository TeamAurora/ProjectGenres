#include "menu_state.h"
#include "game_application.h"
#include <graphics\sprite_renderer.h>
#include <graphics\texture.h>
#include <sstream>
#include <iostream>
#include <audio/vita/audio_manager_vita.h>


MenuState::MenuState(abfw::Platform& platform, const GameApplication* application, abfw::AudioManager* audio_manager) :
	AppState(platform, application, audio_manager),
	main_menu_selection_(START),
	menustate_(MAIN_MENU),
	level_selection_(1),
	options_selection_(MUSIC)
{
	start_button_ = new Button(application_->LoadTextureFromPNG("menu_start_button.png"), application_->LoadTextureFromPNG("menu_start_button_highlighted.png"));
	help_button_ = new Button(application_->LoadTextureFromPNG("menu_help_button.png"), application_->LoadTextureFromPNG("menu_help_button_highlighted.png"));
	options_button_ = new Button(application_->LoadTextureFromPNG("menu_options_button.png"), application_->LoadTextureFromPNG("menu_options_button_highlighted.png"));
	start_button_->set_width(256.0f);
	start_button_->set_height(64.0f);
	start_button_->set_position(abfw::Vector3(750.0f, 190.0f, 0.0f));
	help_button_->set_width(256.0f);
	help_button_->set_height(64.0f);
	help_button_->set_position(abfw::Vector3(783.0f, 290.0f, 0.0f));
	options_button_->set_width(256.0f);
	options_button_->set_height(64.0f);
	options_button_->set_position(abfw::Vector3(717.0f, 400.0f, 0.0f));

	start_button_->Select(true);

	options_buttons_[0] = new Button(application_->LoadTextureFromPNG("options_music.png"), application_->LoadTextureFromPNG("options_music_highlighted.png"));
	options_buttons_[1] = new Button(application_->LoadTextureFromPNG("options_sfx.png"), application_->LoadTextureFromPNG("options_sfx_highlighted.png"));
	music_display_ = new Button(application_->LoadTextureFromPNG("options_music_off.png"), application_->LoadTextureFromPNG("options_music_on.png"));
	sfx_display_ = new Button(application_->LoadTextureFromPNG("options_sfx_off.png"), application_->LoadTextureFromPNG("options_sfx_on.png"));

	music_display_->set_position(670.0f, (platform_.height() / (options_buttons_.size() + 1)) * 1, 0.0f);
	music_display_->set_width(128.0f);
	music_display_->set_height(128.0f);
	music_display_->Select(application_->settings_.music_);

	sfx_display_->set_position(670.0f, (platform_.height() / (options_buttons_.size() + 1)) * 2, 0.0f);
	sfx_display_->set_width(100.0f);
	sfx_display_->set_height(100.0f);
	sfx_display_->Select(application_->settings_.sound_effects_);

	for (int buttonindex = 0; buttonindex < options_buttons_.size(); buttonindex++)
	{
		options_buttons_[buttonindex]->set_width(256.0f);
		options_buttons_[buttonindex]->set_height(64.0f);
		options_buttons_[buttonindex]->set_position(470.0f, (platform_.height() / (options_buttons_.size() + 1)) * (buttonindex + 1), 0.0f);
	}
	options_buttons_[0]->Select(true);

	for (int buttonindex = 0; buttonindex < level_buttons_.size(); buttonindex++)
	{
		std::stringstream default_texture, highlighted_texture;
		default_texture << "level_" << buttonindex + 1 << "_button.png";
		highlighted_texture << "level_" << buttonindex + 1 << "_button_highlighted.png";
		level_buttons_[buttonindex] = new Button(application_->LoadTextureFromPNG(default_texture.str().c_str()), application_->LoadTextureFromPNG(highlighted_texture.str().c_str()));
		switch(buttonindex)
		{
		case 0:
		case 2:
			level_buttons_[buttonindex]->set_width(256.0f);
			break;
		case 1:
			level_buttons_[buttonindex]->set_width(512.0f);
			break;
		}
		level_buttons_[buttonindex]->set_height(128.0f);
		level_buttons_[buttonindex]->set_position((platform_.width() / (level_buttons_.size() + 1)) * (buttonindex + 1), (platform_.height() / 2.0f) + 32.0f, 0.0f);
	}
	level_buttons_[0]->Select(true);
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

	main_menu_overlay_ = Sprite();
	main_menu_overlay_.InitSprite(platform_.width(), platform_.height(), screen_centre, main_menu_overlay_texture_);

	help_overlay_ = Sprite();
	help_overlay_.InitSprite(platform_.width(), platform_.height(), screen_centre, help_overlay_texture_);
}

void MenuState::TerminateState()
{
	DeleteNull(menu_background_texture_);
	DeleteNull(main_menu_overlay_texture_);
	DeleteNull(help_overlay_texture_);
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
					application_->PlayMenuSelect();
				}
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_DOWN)
				{
					main_menu_selection_ = HELP;
					help_button_->Select(true);
					start_button_->Select(false);
					application_->PlayMenuMove();
				}
				break;
			case HELP:
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_UP)
				{
					main_menu_selection_ = START;
					start_button_->Select(true);
					help_button_->Select(false);
					application_->PlayMenuMove();
				}
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_CROSS)
				{
					menustate_ = HELP_SCREEN;
					application_->PlayMenuSelect();
				}
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_DOWN)
				{
					main_menu_selection_ = OPTIONS;
					options_button_->Select(true);
					help_button_->Select(false);
					application_->PlayMenuMove();
				}
				break;
			case OPTIONS:
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_UP)
				{
					main_menu_selection_ = HELP;
					help_button_->Select(true);
					options_button_->Select(false);
					application_->PlayMenuMove();
				}
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_CROSS)
				{
					menustate_ = OPTIONS_SCREEN;
					application_->PlayMenuSelect();
				}
				break;
			}
			break;
		case HELP_SCREEN:
			if (controller->buttons_pressed() & ABFW_SONY_CTRL_CIRCLE)
			{
				menustate_ = MAIN_MENU;
				application_->PlayMenuBack();
			}
			break;
		case OPTIONS_SCREEN:
			if (controller->buttons_pressed() & ABFW_SONY_CTRL_CIRCLE)
			{
				menustate_ = MAIN_MENU;
				application_->PlayMenuBack();
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
					music_display_->Select(application_->settings_.music_);
					application_->PlayMenuSelect();
				}
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_DOWN)
				{
					options_selection_ = SFX;
					options_buttons_[1]->Select(true);
					options_buttons_[0]->Select(false);
					application_->PlayMenuMove();
				}
				break;
			case SFX:
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_UP)
				{
					options_selection_ = MUSIC;
					options_buttons_[0]->Select(true);
					options_buttons_[1]->Select(false);
					application_->PlayMenuMove();
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
					sfx_display_->Select(application_->settings_.sound_effects_);
					application_->PlayMenuSelect();
				}
				break;
			}
			break;
		case LEVEL_SELECT:
			if (level_selection_ > 1)
			{
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_LEFT)
				{
					level_buttons_[level_selection_-1]->Select(false);
					level_selection_--;
					level_buttons_[level_selection_-1]->Select(true);
					application_->PlayMenuMove();
				}
			}

			if (level_selection_ < level_buttons_.size())
			{
				if (controller->buttons_pressed() & ABFW_SONY_CTRL_RIGHT)
				{
					level_buttons_[level_selection_-1]->Select(false);
					level_selection_++;
					level_buttons_[level_selection_-1]->Select(true);
					application_->PlayMenuMove();
				}
			}

			if (controller->buttons_pressed() & ABFW_SONY_CTRL_CROSS)
			{
				application_->PlayMenuSelect();
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
				application_->PlayMenuBack();
			}
			break;
		}
	}
	return MENU;
}

void MenuState::Render(const float frame_rate_, abfw::Font& font_, abfw::SpriteRenderer* sprite_renderer_)
{
	sprite_renderer_->DrawSprite(background_);
	switch (menustate_)
	{
	case MAIN_MENU:
		sprite_renderer_->DrawSprite(main_menu_overlay_);
		sprite_renderer_->DrawSprite(*start_button_);
		sprite_renderer_->DrawSprite(*help_button_);
		sprite_renderer_->DrawSprite(*options_button_);
		break;
	case LEVEL_SELECT:
		for (int buttonindex = 0; buttonindex < level_buttons_.size(); buttonindex++)
		{
			sprite_renderer_->DrawSprite(*level_buttons_[buttonindex]);
		}
		break;
	case OPTIONS:
		for (int buttonindex = 0; buttonindex < options_buttons_.size(); buttonindex++)
		{
			sprite_renderer_->DrawSprite(*options_buttons_[buttonindex]);
		}
		sprite_renderer_->DrawSprite(*music_display_);
		sprite_renderer_->DrawSprite(*sfx_display_);
		break;
	case HELP:
		sprite_renderer_->DrawSprite(help_overlay_);
		break;
	}
}

void MenuState::LoadTextures()
{
	menu_background_texture_ = application_->LoadTextureFromPNG("menu_background.png");
	main_menu_overlay_texture_ = application_->LoadTextureFromPNG("main_menu_background.png");
	help_overlay_texture_ = application_->LoadTextureFromPNG("help_overlay.png");
}