#ifndef _GAME_APPLICATION_H
#define _GAME_APPLICATION_H
#include <system/application.h>
#include <graphics/sprite.h>
#include <graphics/font.h>
#include <input/sony_controller_input_manager.h>
#include "appstate.h"				// include required for APPSTATE to be defined
#include "game_settings.h"
#include "game_object.h"
#include "camera.h"

namespace abfw
{
	class Platform;
	class Texture;
	class AudioManager;
}

class IntroState;
class Level_1;
class Level_2;
class Level_3;
class MenuState;
//Craig
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
	void PlayMenuBack() const;
	void PlayMenuMove() const;
	void PlayMenuSelect() const;

	mutable GameSettings settings_; // easily accessed through GameApplication pointers when public - useful to have settings accessible from anywhere within the application
	Camera* main_camera_; // must be pointer as cannot be constructed until renderer is created
	Camera* player_camera_; // need to do this here to circumvent the fact that appstates don't have access to sprite_renderer_ during initialization

private:

	void ChangeState(APPSTATE next_state); // handles changing states, initializing new state and terminating old state - frame counter is reset to 0 on state change

	// Engine Objects (External abfw related objects)

	abfw::AudioManager* audio_manager_;
	abfw::SpriteRenderer* sprite_renderer_;
	abfw::SonyControllerInputManager* controller_manager_;

	// Resource Objects (Font, Textures, etc)

	abfw::Font font_;
	abfw::Texture* loading_texture_;

	Int32 menu_move_;
	Int32 menu_select_;
	Int32 menu_back_;

	// Application Objects (Objects that are at the application level of the project hierarchy)

	Sprite loading_;

	AppState* pCurrentState;	// pointer to current appstate class
	IntroState* pIntro;			// pointer to each possible state
	MenuState* pMenu;
	Level_1* pLevel_1;
	Level_2* pLevel_2;
	Level_3* pLevel_3;

	APPSTATE gamestate_;		// tracks current state
	APPSTATE updated_state_;	// tracks state to change to (for comparing with current state)
	bool change_state_; // used to draw loading screen before changing state
	float frame_rate_;
	int frame_counter_; // Tracks number of frames since state change - required for exact divisibility for some operations (can't use ticks or frame_rate_)
};

#endif // _GAME_APPLICATION_H