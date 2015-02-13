#include "game_state.h"
#include <graphics/sprite_renderer.h>
#include <audio/vita/audio_manager_vita.h>
#include <graphics/texture.h>
#include <iostream>
#include "game_application.h"
#include "box2d_helpers.h"

GameState::GameState(abfw::Platform& platform, const GameApplication* application, abfw::AudioManager* audio_manager) :
	AppState(platform, application, audio_manager)
{
}


GameState::~GameState()
{
}

void GameState::InitializeState()
{
	LoadTextures();
	LoadSounds();

	// Box2D Init
	// create new box2d world with zero gravity
	world_ = new b2World(b2Vec2(0.0f, 0.0f));

	// Do any other init here
	// Init Objects


}

void GameState::TerminateState()
{
	// Cleanup resources and pointers
	delete world_;
	world_ = NULL;
}

GAMESTATE GameState::Update(const float& ticks_, const int& frame_counter_, const abfw::SonyControllerInputManager& controller_manager_)
{
	// Step box2d world - add if box2d is enabled conditional
	float time_step = 1.0f / 60.0f;
	int32 velocity_iterations = 10;
	int32 position_iterations = 5;
	world_->Step(time_step, velocity_iterations, position_iterations);

	// Handles collision detection
	CollisionDetectionLoop();

	// Deletes non-visible objects (visibility is used to flag for deletion and rendering), updates positions and spawns new objects based on game logic
	UpdateGameObjects(ticks_, frame_counter_);

	// Do input
	const abfw::SonyController* controller = controller_manager_.GetController(0);
	if(controller) // if controller isn't null
	{
		if (controller->buttons_down() & ABFW_SONY_CTRL_START)
		{
			return MENU; // go to menu if start is down
		}
		else // do input loop for state if we aren't returning to menustate
			InputLoop(controller);
	}

	return GAME; // else continue with gamestate
}

void GameState::Render(const float frame_rate_, abfw::Font& font_, abfw::SpriteRenderer* sprite_renderer_)
{
	// Draw stuff

	font_.RenderText(sprite_renderer_, abfw::Vector3(10.0f, 5.0f, -0.9f), 1.0f, 0xff0000ff, abfw::TJ_LEFT, "Generic Space Game");
	font_.RenderText(sprite_renderer_, abfw::Vector3(10.0f, 25.0f, -0.9f), 1.0f, 0xff00ffff, abfw::TJ_LEFT, "FPS: %.1f", frame_rate_);
}

void GameState::LoadTextures()
{
	//Load textures using application_->LoadTextureFromPNG("texturename.png")

	// single object texture loaded directly into objects
	player_.set_texture(application_->LoadTextureFromPNG("Character.png"));
	background_.set_texture(application_->LoadTextureFromPNG("Level_BG.png"));

	// shared textures 
	redPUTex = application_->LoadTextureFromPNG("Red.png");
	bluePUTex = application_->LoadTextureFromPNG("Blue.png");
	platformTex = application_->LoadTextureFromPNG("Platform_Panel.png");
	plantWallTex = application_->LoadTextureFromPNG("Plant_Wall.png");
	plantBlockTex = application_->LoadTextureFromPNG("Plant_Block.png");
}

void GameState::LoadSounds()
{
	// Load audio using audio_manager_->LoadSample("sfxname.wav", platform_)

}

void GameState::InputLoop(const abfw::SonyController* controller)
{
	if(controller->buttons_down() & ABFW_SONY_CTRL_UP) // while up button is down
	{
	}
		
	if(controller->buttons_down() & ABFW_SONY_CTRL_RIGHT) // while right button is down
	{
	}

	if(controller->buttons_down() & ABFW_SONY_CTRL_DOWN) // while down button is down
	{
	}

	if(controller->buttons_down() & ABFW_SONY_CTRL_LEFT) // while left button is down
	{
	}

	if(controller->buttons_pressed() & ABFW_SONY_CTRL_CROSS) // when cross gets pressed
	{
	}

	if(controller->buttons_pressed() & ABFW_SONY_CTRL_SQUARE) // when square gets pressed
	{
	}

	if(controller->buttons_down() & ABFW_SONY_CTRL_L2) // while left shoulder is down
	{
	}

	if(controller->buttons_down() & ABFW_SONY_CTRL_R2) // while right shoulder is down
	{
	}
}

void GameState::UpdateGameObjects(const float& ticks_, const int& frame_counter_)
{
	// Any per-frame or per-tick updating for game objects should be done here

}

void GameState::CollisionDetectionLoop()
{
	b2Contact* contact = world_->GetContactList();
	int contact_count = world_->GetContactCount();
	for(int contact_num = 0; contact_num < contact_count; contact_num++)
	{
		// get pointer to objects that are colliding (must cast void ptr to correct type)
		GameObject* objectA = (GameObject*)contact->GetFixtureA()->GetBody()->GetUserData();
		GameObject* objectB = (GameObject*)contact->GetFixtureB()->GetBody()->GetUserData();
		
		if(contact->IsTouching())
		{
			contact = contact->GetNext();
		}
	}
}