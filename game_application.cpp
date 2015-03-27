#include "game_application.h"
#include <graphics/sprite_renderer.h>
#include <audio/vita/audio_manager_vita.h>
#include <system/platform.h>
#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <graphics/texture.h>
#include <iostream>
#include <ctime>
#include "intro_state.h"
#include "menu_state.h"
#include "level_1.h"
#include "level_2.h"


GameApplication::GameApplication(abfw::Platform& platform) :
	abfw::Application(platform),
	sprite_renderer_(NULL),
	controller_manager_(NULL),
	pCurrentState(NULL),
	change_state_(false)
{
	ChangeState(INTRO);
}

GameApplication::~GameApplication()
{
}

void GameApplication::Init()
{
	// load the font to draw the on-screen text
	bool font_loaded = font_.Load("comic_sans", platform_);
	if(!font_loaded)
	{
		std::cout << "Font failed to load." << std::endl;
		exit(-1);
	}

	// Initialize platform-specific systems
	sprite_renderer_ = platform_.CreateSpriteRenderer();
	controller_manager_ = platform_.CreateSonyControllerInputManager();
	audio_manager_ = new abfw::AudioManagerVita;
	camera_ = new Camera(sprite_renderer_, platform_);
	
	// Seed RNG
	srand(time(NULL));
	
	// Initialize state-shared resources
	loading_texture_ = LoadTextureFromPNG("loading_background.png");
	loading_.InitSprite(platform_.width(), platform_.height(), abfw::Vector3(platform_.width() / 2.0f, platform_.height() / 2.0f, 0.0f), loading_texture_);
	//audio_manager_->LoadMusic("musicname.wav", platform_);
}

void GameApplication::CleanUp()
{
	// Resources cleanup
	DeleteNull(loading_texture_);
	audio_manager_->UnloadMusic();
	
	// State machine cleanup
	DeleteNull(pIntro);
	DeleteNull(pMenu);
	DeleteNull(pLevelSelect);
	DeleteNull(pLevel_1);
	DeleteNull(pLevel_2);
	DeleteNull(pLevel_3);
	DeleteNull(pScoreScreen);
	pCurrentState = NULL; // Cannot delete the interface pointer (it is never instantiated)

	// Application cleanup
	DeleteNull(camera_);
	DeleteNull(controller_manager_);
	DeleteNull(sprite_renderer_);
	DeleteNull(audio_manager_);
}

bool GameApplication::Update(float ticks)
{
	// calculate the frame rate and increment total frame counter
	frame_rate_ = 1.0f / ticks;
	frame_counter_++;

	bool music_playing_ = settings_.music_; // temp variables which hold last frames music status
	
	// Update controller_manager_ for input
	controller_manager_->Update();
	
	if(change_state_ == true) // if flagged for state change
	{
		ChangeState(updated_state_); // changes to new state
	}
	
	updated_state_ = pCurrentState->Update(ticks, frame_counter_, *controller_manager_); // update state and return state for next frame
	if(updated_state_ != gamestate_) // if the state returned from update is not the same as the current state
	{
		change_state_ = true; // flag for state change (render will render a loading screen this frame and state will change next frame)
	}
	else
	{
		change_state_ = false; // else make state change false
	}

	if(settings_.music_ != music_playing_)	// if last frames music setting isn't equal to this frames
	{
		if(settings_.music_)				// then if music has changed to true
		{
			audio_manager_->PlayMusic();	// start playing
		}
		else								// else it has changed to not playing
		{
			audio_manager_->StopMusic();	// stop playing
		}
	}

	camera_->ApplyCameraTransforms(ticks);

	return true;
}

void GameApplication::Render()
{
	// set up sprite renderer for drawing
	sprite_renderer_->Begin();

	if(change_state_)
	{
		sprite_renderer_->DrawSprite(loading_); // draw load screen if state changes next frame
	}
	else
	{
		pCurrentState->Render(frame_rate_, font_, sprite_renderer_); // else do state-specific rendering
	}

	// tell sprite renderer that all sprites have been drawn
	sprite_renderer_->End();
}

abfw::Texture* GameApplication::LoadTextureFromPNG(const char* filename) const
{
	abfw::PNGLoader png_loader;
	abfw::ImageData image_data;
	png_loader.Load(filename, platform_, image_data);
	
	if(image_data.image() == NULL)
	{
		return NULL;
	}
	else
	{
		abfw::Texture* texture;
		texture = platform_.CreateTexture(image_data);
		return texture;
	}
}

void GameApplication::ChangeState(APPSTATE next_state)
{
	if(pCurrentState != NULL) 				// if pCurrentState points somewhere
	{
		pCurrentState->TerminateState(); 	// terminates previous state
		switch(gamestate_)					// destructs and deallocates memory for previous state, sets pointer of previous state to NULL
		{
			case INTRO: DeleteNull(pIntro);
						break;
			case MENU: 	DeleteNull(pMenu);
						break;
			case LEVEL_SELECT: 	DeleteNull(pLevelSelect);
								break;
			case LEVEL_1: 	DeleteNull(pLevel_1);
							break;
			case LEVEL_2: 	DeleteNull(pLevel_2);
							break;
			case LEVEL_3: 	DeleteNull(pLevel_3);
							break;
			case SCORE_SCREEN: 	DeleteNull(pScoreScreen);
								break;
		}
	}
	
	switch(next_state)	// construct and allocate memory for new state, sets current state pointer to new state
	{
		case INTRO: pIntro = new IntroState(platform_, this, audio_manager_);
					pCurrentState = pIntro;
					break;
		case MENU: 	pMenu = new MenuState(platform_, this, audio_manager_);
					pCurrentState = pMenu;
					break;
		case LEVEL_SELECT: 	//pLevelSelect = new LevelSelect(platform_, this, audio_manager_); NYI
							//pCurrentState = pLevelSelect;
							break;
		case LEVEL_1: 	pLevel_1 = new Level_1(platform_, this, audio_manager_);
						pCurrentState = pLevel_1;
						break;
		case LEVEL_2: 	pLevel_2 = new Level_2(platform_, this, audio_manager_); NYI
						pCurrentState = pLevel_2;
						break;
		case LEVEL_3: 	pLevel_3 = new Level_3(platform_, this, audio_manager_); NYI
						pCurrentState = pLevel_3;
						break;
		case SCORE_SCREEN: 	pScoreScreen = new ScoreScreen(platform_, this, audio_manager_); NYI
							pCurrentState = pScoreScreen;
							break;
	}

	pCurrentState->InitializeState();		// initialize new state
	gamestate_ = next_state;
	frame_counter_ = 0;						// resets frame_counter_ when state changes
}