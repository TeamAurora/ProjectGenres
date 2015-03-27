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

	enum BUTTONSELECTION { START, HELP, OPTIONS };
	BUTTONSELECTION selection_;

	int level_selection_;

	enum RENDERSTATE { MAIN_MENU, HELP_SCREEN, OPTIONS_SCREEN, LEVEL_SELECT };
	RENDERSTATE render_;
	
	void LoadTextures();
	
	abfw::Texture* main_menu_background_texture_;
	abfw::Texture* help_screen_background_texture_;
	abfw::Texture* options_screen_background_texture_;
	abfw::Texture* level_select_background_texture_;
	Sprite background_;
	Button start_button_;
	Button help_button_;
	Button options_button_;
	Button level_buttons[3];
};

