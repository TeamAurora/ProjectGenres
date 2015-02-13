#pragma once
#include "appstate.h"
#include "game_object.h"

class MenuState :
	public AppState
{
public:
	MenuState(abfw::Platform& platform, const GameApplication* application, abfw::AudioManager* audio_manager);
	virtual ~MenuState();
	
	void InitializeState();
	void TerminateState();
	GAMESTATE Update(const float& ticks_, const int& frame_counter_, const abfw::SonyControllerInputManager& controller_manager_);
	void Render(const float frame_rate_, abfw::Font& font_, abfw::SpriteRenderer* sprite_renderer_);
	
private:
	
	void LoadTextures();

	enum SELECTION { STARTGAME, MUSIC, SOUNDEFFECTS, DIFFICULTY}; // possible menu selections
	SELECTION selection_;	// current menu selection
	
	abfw::Texture* start_texture_;
	abfw::Texture* cross_texture_;
	abfw::Texture* music_on_texture_;
	abfw::Texture* music_off_texture_;
	abfw::Texture* sound_on_texture_;
	abfw::Texture* sound_off_texture_;
	abfw::Texture* dpad_texture_;
	abfw::Texture* blue_square_;
	abfw::Texture* green_square_;
	abfw::Texture* yellow_diamond_;
	abfw::Texture* red_diamond_;
	abfw::Texture* purple_polygon_;
	abfw::Texture* selector_;
	GameObject start_button_;
	GameObject music_button_;
	GameObject sound_effects_button_;
	//GameObject difficulty_button[kDifficultySettings];
	GameObject selection_pointer_;
	GameObject dpad_;
	GameObject cross_;
};

