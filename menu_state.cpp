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
	LoadTextures();

	abfw::Vector3 screen_centre(platform_.width()/2.0f, platform_.height()/2.0f, 0.0f);
	
	background_.InitSprite(platform_.width(), platform_.height(), screen_centre, background_texture_);

	start_button_ = Button();
	start_button_.InitSprite(201.0f, 66.0f, abfw::Vector3(750.0f, 190.0f, 0.0f), start_button_highlighted_texture_);

	help_button_ = Button();
	help_button_.InitSprite(166.0f, 57.0f, abfw::Vector3(783.0f, 290.0f, 0.0f), help_button_texture_);

	options_button_ = Button();
	options_button_.InitSprite(246.0f, 57.0f, abfw::Vector3(717.0f, 400.0f, 0.0f), options_button_texture_);
	
	// Difficulty Buttons
	// Each buttons position is relative to the last buttons with the exception
	// of the first which is moved to position relative to the centre of the screen.
	/*float diff_button_spacing = (platform_.width() * 0.5f) / (float)kDifficultySettings;
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
	cross_.InitSprite(128.0f, 128.0f, abfw::Vector3(platform_.width() * 0.9f, platform_.height() * 0.8f, 0.0f), cross_texture_);*/
}
void MenuState::TerminateState()
{
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
		if (controller->buttons_down() & ABFW_SONY_CTRL_CROSS)
		{
			return LEVEL_2;
		}
	}
	
	return MENU;
}
void MenuState::Render(const float frame_rate_, abfw::Font& font_, abfw::SpriteRenderer* sprite_renderer_)
{
	sprite_renderer_->DrawSprite(background_);

	sprite_renderer_->DrawSprite(start_button_);

	sprite_renderer_->DrawSprite(help_button_);

	sprite_renderer_->DrawSprite(options_button_);
}
void MenuState::LoadTextures()
{
	background_texture_ = application_->LoadTextureFromPNG("menu_background.png");
	start_button_texture_ = application_->LoadTextureFromPNG("start_button.png");
	help_button_texture_ = application_->LoadTextureFromPNG("help_button.png");
	options_button_texture_ = application_->LoadTextureFromPNG("options_button.png");
	start_button_highlighted_texture_ = application_->LoadTextureFromPNG("start_button_highlighted.png");
	help_button_highlighted_texture_ = application_->LoadTextureFromPNG("help_button_highlighted.png");
	options_button_highlighted_texture_ = application_->LoadTextureFromPNG("options_button_highlighted.png");
}