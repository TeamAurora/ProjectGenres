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
	
	void LoadTextures();
	
	abfw::Texture* background_texture_;
	abfw::Texture* start_button_texture_;
	abfw::Texture* start_button_highlighted_texture_;
	abfw::Texture* help_button_texture_;
	abfw::Texture* help_button_highlighted_texture_;
	abfw::Texture* options_button_texture_;
	abfw::Texture* options_button_highlighted_texture_;
	Sprite background_;
	Button start_button_;
	Button help_button_;
	Button options_button_;
};

