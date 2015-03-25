#pragma once
#include "appstate.h"
#include "Button.h"

class MenuState :
	public AppState
{
public:
	MenuState(abfw::Platform& platform, const GameApplication* application, abfw::AudioManager* audio_manager);
	virtual ~MenuState();
	
	void InitializeState();
	void TerminateState();
	APPSTATE Update(const float& ticks_, const int& frame_counter_, const abfw::SonyControllerInputManager& controller_manager_);
	void Render(const float frame_rate_, abfw::Font& font_, abfw::SpriteRenderer* sprite_renderer_);
	
private:

	enum SELECTION { START, HELP, OPTIONS };
	SELECTION selection_;

	int level_selection_;

	enum MENUSTATE { MAIN_MENU, HELP_SCREEN, OPTIONS_SCREEN, LEVEL_SELECT };
	MENUSTATE state_;
	
	void LoadTextures();
	
	abfw::Texture* main_menu_background_texture_;
	abfw::Texture* help_screen_background_texture_;
	abfw::Texture* options_screen_background_texture_;
	abfw::Texture* level_select_background_texture_;
	abfw::Texture* start_button_texture_;
	abfw::Texture* start_button_highlighted_texture_;
	abfw::Texture* help_button_texture_;
	abfw::Texture* help_button_highlighted_texture_;
	abfw::Texture* options_button_texture_;
	abfw::Texture* options_button_highlighted_texture_;
	abfw::Texture* level_buttons_texture[6]; // 6 textures for 3 buttons (alternating between non-highlighted and highlighted textures)
	Sprite background_;
	Button start_button_;
	Button help_button_;
	Button options_button_;
	Button level_buttons[3];
};

