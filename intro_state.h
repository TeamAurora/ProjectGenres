#pragma once
#include "appstate.h"
#include "game_object.h"

class IntroState :
	public AppState
{
public:
	IntroState(abfw::Platform& platform, const GameApplication* application, abfw::AudioManager* audio_manager);
	virtual ~IntroState();
	
	void InitializeState();
	void TerminateState();
	GAMESTATE Update(const float& ticks_, const int& frame_counter_, const abfw::SonyControllerInputManager& controller_manager_);
	void Render(const float frame_rate_, abfw::Font& font_, abfw::SpriteRenderer* sprite_renderer_);

private:
	void LoadTextures();

	abfw::Texture* splash_texture_;
	GameObject splash_;
};