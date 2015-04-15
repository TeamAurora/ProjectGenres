#pragma once
#include "appstate.h"
#include "Button.h"
#include <array>

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

	enum MAINMENUBUTTONSELECTION { START, HELP, OPTIONS };
	MAINMENUBUTTONSELECTION main_menu_selection_;

	enum OPTIONSBUTTONSELECTION { MUSIC, SFX };
	OPTIONSBUTTONSELECTION options_selection_;

	int level_selection_;

	enum MENUSTATE { MAIN_MENU, HELP_SCREEN, OPTIONS_SCREEN, LEVEL_SELECT };
	MENUSTATE menustate_;
	
	void LoadTextures();
	abfw::Texture* menu_background_texture_;
	abfw::Texture* main_menu_texture_;
	abfw::Texture* help_screen_texture_;
	abfw::Texture* options_screen_texture_;
	abfw::Texture* level_select_texture_;
	Sprite background_;
	Sprite background_overlay_;
	Button start_button_;
	Button help_button_;
	Button options_button_;
	std::array<Button, 2> options_buttons_;
	std::array<Button, 3> level_buttons_;
};

