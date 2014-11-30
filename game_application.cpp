#include "game_application.h"
#include <graphics/sprite_renderer.h>
#include <audio/vita/audio_manager_vita.h>
#include <system/platform.h>
#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <graphics/texture.h>
#include <iostream>
#include "game_state.h"
#include "intro_state.h"
#include "menu_state.h"


GameApplication::GameApplication(abfw::Platform& platform) :
	abfw::Application(platform),
	sprite_renderer_(NULL),
	controller_manager_(NULL)
{
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

	// Create sprite renderer
	sprite_renderer_ = platform_.CreateSpriteRenderer();

	// Create controller manager
	controller_manager_ = platform_.CreateSonyControllerInputManager();

	// Create audio manager
	audio_manager_ = new abfw::AudioManagerVita;
	
	// Seed RNG
	srand (5189023); // random number until I find out about time(null) include
	
	// Loading Screen Initialization
	loading_texture_ = LoadTextureFromPNG("Load Screen.png");
	loading_.InitSprite(platform_.width(), platform_.height(), abfw::Vector3(platform_.width()/2.0f, platform_.height()/2.0f, 0.0f), loading_texture_);
	
	// Initializes game settings
	settings_.music_ = true;
	settings_.sound_effects_ = true;
	settings_.difficulty_ = GameSettings::MEDIUM;
	
	// Initializes state machine to INTRO state (changestate also renders a loading screen)
	pCurrentState = NULL;
	ChangeState(INTRO);
	change_state_ = false;
	
	// Background Initialization
	background_texture_ = LoadTextureFromPNG("background_black.png");
	background_.InitSprite(platform_.width(), platform_.height(), abfw::Vector3(platform_.width()/2.0f, platform_.height()/2.0f, 0.0f), background_texture_);

	// Load Music
	audio_manager_->LoadMusic("Electric_Quake.wav", platform_); // http://opengameart.org/content/electric-quake
	audio_manager_->PlayMusic();
}

void GameApplication::CleanUp()
{
	// Application Cleanup
	delete controller_manager_;
	controller_manager_ = NULL;

	delete sprite_renderer_;
	sprite_renderer_ = NULL;

	audio_manager_->UnloadMusic();
	delete audio_manager_;
	audio_manager_ = NULL;
	
	// Textures
	delete background_texture_;
	background_texture_ = NULL;
	delete loading_texture_;
	loading_texture_ = NULL;
	
	// State Machine
	if(pIntro != NULL)
		delete pIntro;
	if(pMenu != NULL)
		delete pMenu;
	if(pGame != NULL)
		delete pGame;
	pCurrentState = NULL;
	pIntro = NULL;
	pMenu = NULL;
	pGame = NULL;
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
		switch(settings_.music_)			// start or stop music appropriately
		{
		case true:
			audio_manager_->PlayMusic();
			break;
		case false:
			audio_manager_->StopMusic();
			break;
		}
	}

	return true;
}

void GameApplication::Render()
{
	// set up sprite renderer for drawing
	sprite_renderer_->Begin();

	switch(change_state_)
	{
	case true:
		sprite_renderer_->DrawSprite(loading_); // draw load screen if state changes this frame
		break;
	case false:
		// Background
		sprite_renderer_->DrawSprite(background_);
		pCurrentState->Render(frame_rate_, font_, sprite_renderer_); // don't need to do state specific rendering if we're going to change state next frame - loading screen gets drawn instead
		break;
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

void GameApplication::ChangeState(GAMESTATE next_state)
{
	if(pCurrentState != NULL) 				// if pCurrentState points somewhere
	{
		pCurrentState->TerminateState(); 	// terminates previous state
		switch(gamestate_)					// destructs and deallocates memory for previous state, sets pointer of previous state to NULL
		{
			case INTRO: delete pIntro;
						pIntro = NULL;
						break;
			case MENU: 	delete pMenu;
						pMenu = NULL;
						break;
			case GAME: 	delete pGame;
						pGame = NULL;
						break;
		}
	}
	
	switch(next_state)						// construct and allocate memory for new state, sets current state pointer to new state
	{
		case INTRO: pIntro = new IntroState(platform_, this, audio_manager_);
					pCurrentState = pIntro;
					break;
		case MENU: 	pMenu = new MenuState(platform_, this, audio_manager_);
					pCurrentState = pMenu;
					break;
		case GAME: 	pGame = new GameState(platform_, this, audio_manager_);
					pCurrentState = pGame;
					break;
	}

	pCurrentState->InitializeState();		// initialize new state
	gamestate_ = next_state;
	frame_counter_ = 0;						// resets frame_counter_ when state changes
}