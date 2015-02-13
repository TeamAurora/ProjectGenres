#include "menu_state.h"
#include <graphics/sprite_renderer.h>
#include <audio/vita/audio_manager_vita.h>
#include <system/platform.h>
#include <graphics/texture.h>
#include "game_application.h"


MenuState::MenuState(abfw::Platform& platform, const GameApplication* application, abfw::AudioManager* audio_manager) :
	AppState(platform, application, audio_manager)
{
}


MenuState::~MenuState()
{
}

void MenuState::InitializeState()
{
	/*LoadTextures();

	abfw::Vector3 screen_centre(platform_.width()/2.0f, platform_.height()/2.0f, 0.0f);
	
	// Start Button
	start_button_ = GameObject();	// construct start button
	start_button_.InitSprite(256.0f, 128.0f, screen_centre, start_texture_);
	
	// Sound Controls
	float sound_button_spacing = platform_.width() * 0.2f;
	music_button_ = GameObject();	// construct music button
	music_button_.InitSprite(38.0f, 36.0f, screen_centre, music_on_texture_);
	music_button_.MoveBy(-0.5f * sound_button_spacing, -(platform_.height() * 0.2f)); // moves left by half the button spacing and up by 1/5th the platform height
	sound_effects_button_ = GameObject();	// construct sound effects button
	sound_effects_button_.InitSprite(38.0f, 36.0f, music_button_.position(), sound_on_texture_);
	sound_effects_button_.MoveBy(sound_button_spacing, 0.0f);
	
	// Difficulty Buttons
	// Each buttons position is relative to the last buttons with the exception
	// of the first which is moved to position relative to the centre of the screen.
	float diff_button_spacing = (platform_.width() * 0.5f) / (float)kDifficultySettings;
	difficulty_button[0] = GameObject();	// construct first difficulty button
	difficulty_button[0].InitSprite(48.0f, 36.0f, screen_centre, green_square_);
	difficulty_button[0].MoveBy(-((float)kDifficultySettings / 2.5f) * diff_button_spacing, platform_.height() * 0.3f); // moves left by half the number of buttons times spacing and down by 1/5th the platform height
	for(int i = 1; i < kDifficultySettings; i++)
	{
		difficulty_button[i] = GameObject();
		difficulty_button[i].InitSprite(48.0f, 36.0f, difficulty_button[i-1].position(), blue_square_);
		difficulty_button[i].MoveBy(diff_button_spacing, 0.0f);
	}
	difficulty_button[2].set_texture(yellow_diamond_); // set textures to correct textures (2nd button texture is used initially for all buttons after first)
	difficulty_button[3].set_texture(red_diamond_);
	difficulty_button[4].set_texture(purple_polygon_);
	
	// Selection Pointer
	// Initialized to screen centre position(irrelevant as it gets changed to correct position in update loop before render)
	selection_pointer_ = GameObject();
	selection_pointer_.InitSprite(64.0f, 64.0f, screen_centre, selector_);
	
	// Dpad
	dpad_ = GameObject();
	dpad_.InitSprite(128.0f, 128.0f, abfw::Vector3(platform_.width() * 0.1f, platform_.height() * 0.8f, 0.0f), dpad_texture_);
	
	// Cross
	cross_ = GameObject();
	cross_.InitSprite(128.0f, 128.0f, abfw::Vector3(platform_.width() * 0.9f, platform_.height() * 0.8f, 0.0f), cross_texture_);

	selection_ = STARTGAME; // starts selection as startgame button*/

	// NOTE - Visibility is not set to true for any sprites (it's default initialized to false) as the render function of
	// this state won't check for visibility before rendering anyway because the whole menu will be rendered every frame.
}
void MenuState::TerminateState()
{
	/*delete start_texture_;
	start_texture_ = NULL;
	delete cross_texture_;
	cross_texture_ = NULL;
	delete music_on_texture_;
	music_on_texture_ = NULL;
	delete music_off_texture_;
	music_off_texture_ = NULL;
	delete sound_on_texture_;
	sound_on_texture_ = NULL;
	delete sound_off_texture_;
	sound_off_texture_ = NULL;
	delete dpad_texture_;
	dpad_texture_ = NULL;
	delete blue_square_;
	blue_square_ = NULL;
	delete green_square_;
	green_square_ = NULL;
	delete yellow_diamond_;
	yellow_diamond_ = NULL;
	delete red_diamond_;
	red_diamond_ = NULL;
	delete purple_polygon_;
	purple_polygon_ = NULL;
	delete selector_;
	selector_ = NULL;*/
}
GAMESTATE MenuState::Update(const float& ticks_, const int& frame_counter_, const abfw::SonyControllerInputManager& controller_manager_)
{
	/*const abfw::SonyController* controller = controller_manager_.GetController(0); // get the platform specific controller from controller_manager
	if(controller) // if controller isn't null
	{
		switch(selection_) // do input based on what button is currently selected
		{
		case STARTGAME:
			if(controller->buttons_pressed() & ABFW_SONY_CTRL_CROSS)	// if cross is pressed while startgame is selected
			{
				return GAME;											// return state to change to as GAME
			}
			if(controller->buttons_pressed() & ABFW_SONY_CTRL_DOWN)	// if down is pressed while startgame is selected
			{
				selection_ = DIFFICULTY;								// change selection to difficulty
			}
			if(controller->buttons_pressed() & ABFW_SONY_CTRL_UP)		// if up is pressed while startgame is selected
			{
				selection_ = MUSIC;										// change selection to music
			}
			selection_pointer_.MoveTo(start_button_.position().x, start_button_.position().y);
			selection_pointer_.set_width(start_button_.width());
			selection_pointer_.set_height(start_button_.height());
			break;
		case MUSIC:
			if(controller->buttons_pressed() & ABFW_SONY_CTRL_CROSS)	// if cross is pressed while music is selected
			{
				switch(application_->settings_.music_)
				{
				case true:
					application_->settings_.music_ = false;				// set music to false if true
					music_button_.set_texture(music_off_texture_);
					break;
				case false:
					application_->settings_.music_ = true;				// set music to true if false
					music_button_.set_texture(music_on_texture_);
					break;
				}
			}
			if(controller->buttons_pressed() & ABFW_SONY_CTRL_RIGHT)	// if right is pressed while music is selected
			{
				selection_ = SOUNDEFFECTS;								// change selection to soundeffects
			}
			if(controller->buttons_pressed() & ABFW_SONY_CTRL_DOWN)	// if down is pressed while music is selected
			{
				selection_ = STARTGAME;									// change selection to startgame
			}
			selection_pointer_.MoveTo(music_button_.position().x, music_button_.position().y);
			selection_pointer_.set_width(music_button_.width());
			selection_pointer_.set_height(music_button_.height());
			break;
		case SOUNDEFFECTS:
			if(controller->buttons_pressed() & ABFW_SONY_CTRL_CROSS)	// if cross is pressed while soundeffects is selected
			{
				switch(application_->settings_.sound_effects_)
				{
				case true:
					application_->settings_.sound_effects_ = false;		// set sound_effects_ to false if true
					sound_effects_button_.set_texture(sound_off_texture_);
					break;
				case false:
					application_->settings_.sound_effects_ = true;		// set sound_effects_ to true if false
					sound_effects_button_.set_texture(sound_on_texture_);
					break;
				}
			}
			if(controller->buttons_pressed() & ABFW_SONY_CTRL_DOWN)	// if down is pressed while soundeffects is selected
			{
				selection_ = STARTGAME;									// change selection to startgame
			}
			if(controller->buttons_pressed() & ABFW_SONY_CTRL_LEFT)	// if left is pressed while soundeffects is selected
			{
				selection_ = MUSIC;										// change selection to music
			}
			selection_pointer_.MoveTo(sound_effects_button_.position().x, sound_effects_button_.position().y);
			selection_pointer_.set_width(sound_effects_button_.width());
			selection_pointer_.set_height(sound_effects_button_.height());
			break;
		case DIFFICULTY:
			switch(application_->settings_.difficulty_) // do input based on which difficulty is selected
			{
			case GameSettings::EASY:
				if(controller->buttons_pressed() & ABFW_SONY_CTRL_RIGHT)
				{
					application_->settings_.difficulty_ = GameSettings::MEDIUM;
				}
				selection_pointer_.MoveTo(difficulty_button[0].position().x, difficulty_button[0].position().y);
				selection_pointer_.set_width(difficulty_button[0].width());
				selection_pointer_.set_height(difficulty_button[0].height());
				break;
			case GameSettings::MEDIUM:
				if(controller->buttons_pressed() & ABFW_SONY_CTRL_LEFT)
				{
					application_->settings_.difficulty_ = GameSettings::EASY;
				}
				if(controller->buttons_pressed() & ABFW_SONY_CTRL_RIGHT)
				{
					application_->settings_.difficulty_ = GameSettings::HARD;
				}
				selection_pointer_.MoveTo(difficulty_button[1].position().x, difficulty_button[1].position().y);
				selection_pointer_.set_width(difficulty_button[1].width());
				selection_pointer_.set_height(difficulty_button[1].height());
				break;
			case GameSettings::HARD:
				if(controller->buttons_pressed() & ABFW_SONY_CTRL_LEFT)
				{
					application_->settings_.difficulty_ = GameSettings::MEDIUM;
				}
				if(controller->buttons_pressed() & ABFW_SONY_CTRL_RIGHT)
				{
					application_->settings_.difficulty_ = GameSettings::EXTREME;
				}
				selection_pointer_.MoveTo(difficulty_button[2].position().x, difficulty_button[2].position().y);
				selection_pointer_.set_width(difficulty_button[2].width());
				selection_pointer_.set_height(difficulty_button[2].height());
				break;
			case GameSettings::EXTREME:
				if(controller->buttons_pressed() & ABFW_SONY_CTRL_LEFT)
				{
					application_->settings_.difficulty_ = GameSettings::HARD;
				}
				if(controller->buttons_pressed() & ABFW_SONY_CTRL_RIGHT)
				{
					application_->settings_.difficulty_ = GameSettings::TRANSCENDENT;
				}
				selection_pointer_.MoveTo(difficulty_button[3].position().x, difficulty_button[3].position().y);
				selection_pointer_.set_width(difficulty_button[3].width());
				selection_pointer_.set_height(difficulty_button[3].height());
				break;
			case GameSettings::TRANSCENDENT:
				if(controller->buttons_pressed() & ABFW_SONY_CTRL_LEFT)
				{
					application_->settings_.difficulty_ = GameSettings::EXTREME;
				}
				selection_pointer_.MoveTo(difficulty_button[4].position().x, difficulty_button[4].position().y);
				selection_pointer_.set_width(difficulty_button[4].width());
				selection_pointer_.set_height(difficulty_button[4].height());
				break;
			}
			if(controller->buttons_pressed() & ABFW_SONY_CTRL_UP)		// if up is pressed while difficulty is selected
			{
				selection_ = STARTGAME;									// change selection to gamestart
			}
			break;
		}
	}
	return MENU;*/
}
void MenuState::Render(const float frame_rate_, abfw::Font& font_, abfw::SpriteRenderer* sprite_renderer_)
{
	/*// Start Button
	sprite_renderer_->DrawSprite(start_button_);

	// Sound Buttons
	sprite_renderer_->DrawSprite(music_button_);
	font_.RenderText(sprite_renderer_, abfw::Vector3(music_button_.position().x, music_button_.position().y - 2.0f * music_button_.height(), -0.9f), 1.0f, 0xffffffff, abfw::TJ_CENTRE, "Music:");
	sprite_renderer_->DrawSprite(sound_effects_button_);
	font_.RenderText(sprite_renderer_, abfw::Vector3(sound_effects_button_.position().x, sound_effects_button_.position().y - 2.0f * sound_effects_button_.height(), -0.9f), 1.0f, 0xffffffff, abfw::TJ_CENTRE, "Sound Effects:");

	// Difficulty Buttons
	for(int i = 0; i < kDifficultySettings; i++)
	{
		sprite_renderer_->DrawSprite(difficulty_button[i]);
	}
	// Difficulty Text
	switch(application_->settings_.difficulty_)
	{
	case GameSettings::EASY:
		font_.RenderText(sprite_renderer_, abfw::Vector3(platform_.width() * 0.5f, difficulty_button[0].position().y - 2.0f * difficulty_button[0].height(), -0.9f), 1.0f, 0xff00ff00, abfw::TJ_CENTRE, "Current Difficulty: Easy");
		break;
	case GameSettings::MEDIUM:
		font_.RenderText(sprite_renderer_, abfw::Vector3(platform_.width() * 0.5f, difficulty_button[0].position().y - 2.0f * difficulty_button[0].height(), -0.9f), 1.0f, 0xffff0000, abfw::TJ_CENTRE, "Current Difficulty: Medium");
		break;
	case GameSettings::HARD:
		font_.RenderText(sprite_renderer_, abfw::Vector3(platform_.width() * 0.5f, difficulty_button[0].position().y - 2.0f * difficulty_button[0].height(), -0.9f), 1.0f, 0xff00ffff, abfw::TJ_CENTRE, "Current Difficulty: Hard");
		break;
	case GameSettings::EXTREME:
		font_.RenderText(sprite_renderer_, abfw::Vector3(platform_.width() * 0.5f, difficulty_button[0].position().y - 2.0f * difficulty_button[0].height(), -0.9f), 1.0f, 0xff0000ff, abfw::TJ_CENTRE, "Current Difficulty: Extreme");
		break;
	case GameSettings::TRANSCENDENT:
		font_.RenderText(sprite_renderer_, abfw::Vector3(platform_.width() * 0.5f, difficulty_button[0].position().y - 2.0f * difficulty_button[0].height(), -0.9f), 1.0f, 0xffff00ff, abfw::TJ_CENTRE, "Current Difficulty: Transcendent");
		break;
	}

	// Selection Pointer
	sprite_renderer_->DrawSprite(selection_pointer_);
	
	// Dpad
	sprite_renderer_->DrawSprite(dpad_);
	
	// Cross
	sprite_renderer_->DrawSprite(cross_);*/
}
void MenuState::LoadTextures()
{
	/*start_texture_ = application_->LoadTextureFromPNG("transparent_start.png");
	cross_texture_ = application_->LoadTextureFromPNG("transparent_cross.png");
	music_on_texture_ = application_->LoadTextureFromPNG("transparent_music_on.png");
	music_off_texture_ = application_->LoadTextureFromPNG("transparent_music_off.png");
	sound_on_texture_ = application_->LoadTextureFromPNG("transparent_sound_on.png");
	sound_off_texture_ = application_->LoadTextureFromPNG("transparent_sound_off.png");
	dpad_texture_ = application_->LoadTextureFromPNG("transparent_dpad.png");
	blue_square_ = application_->LoadTextureFromPNG("element_blue_square.png");
	green_square_ = application_->LoadTextureFromPNG("element_green_square.png");
	yellow_diamond_ = application_->LoadTextureFromPNG("element_yellow_diamond.png");
	red_diamond_ = application_->LoadTextureFromPNG("element_red_diamond.png");
	purple_polygon_ = application_->LoadTextureFromPNG("element_purple_polygon.png");
	selector_ = application_->LoadTextureFromPNG("selector.png");*/
}