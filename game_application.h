#ifndef _GAME_APPLICATION_H
#define _GAME_APPLICATION_H
#include <system/application.h>
#include <graphics/sprite.h>
#include <graphics/font.h>
#include <input/sony_controller_input_manager.h>
#include "appstate.h"				// include required for GAMESTATE to be defined
#include "game_settings.h"
#include "game_object.h"
#include <vector>

namespace abfw
{
	class Platform;
	class Texture;
	class AudioManager;
}

class IntroState;
class GameState;
class MenuState;

class GameApplication : public abfw::Application
{
public:

	GameApplication(abfw::Platform& platform);
	~GameApplication();
	
	void Init();
	void CleanUp();
	bool Update(float ticks);
	void Render();
	
	abfw::Texture* LoadTextureFromPNG(const char* filename) const; // must be const functions to allow "this" pointers access ("this" pointer is const qualified)
	
	mutable GameSettings settings_; // easily accessed through GameApplication pointers when public - useful to have settings accessible from anywhere within the application
	
private:

	void ChangeState(GAMESTATE next_state); // handles changing states, initializing new state and terminating old state - frame counter is reset to 0 on state change

	// Objects
	abfw::SonyControllerInputManager* controller_manager_;
	abfw::AudioManager* audio_manager_;
	abfw::Font font_;
	abfw::SpriteRenderer* sprite_renderer_;
	abfw::Texture* background_texture_;
	abfw::Texture* star_texture_;
	abfw::Texture* loading_texture_;
	Sprite background_;
	Sprite loading_;
	
	AppState* pCurrentState;	// pointer to current appstate class
	IntroState* pIntro;			// pointer to each possible state
	MenuState* pMenu;
	GameState* pGame;
	
	// Variables
	GAMESTATE gamestate_;		// tracks current state
	GAMESTATE updated_state_;	// tracks state to change to (for comparing with current state)
	bool change_state_; // used to draw loading screen before changing state
	float frame_rate_;
	int frame_counter_; // Tracks number of frames since state change - required for exact divisibility for some operations (can't use ticks or frame_rate_)
};

#endif // _GAME_APPLICATION_H