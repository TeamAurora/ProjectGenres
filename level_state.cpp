#include "level_state.h"
#include <graphics/sprite_renderer.h>
#include <audio/vita/audio_manager_vita.h>
#include <graphics/texture.h>
#include <iostream>
#include "game_application.h"
#include "box2d_helpers.h"

LevelState::LevelState(abfw::Platform& platform, const GameApplication* application, abfw::AudioManager* audio_manager) :
	AppState(platform, application, audio_manager),
	paused_(false)
{
}


LevelState::~LevelState()
{
}

void LevelState::InitializeState()
{
	LoadAssets();
	ConstructBackgroundVectorsFromMap();
	ConstructPhysicsBodiesFromMap();

}

void LevelState::TerminateState()
{

}

APPSTATE LevelState::Update(const float& ticks_, const int& frame_counter_, const abfw::SonyControllerInputManager& controller_manager_)
{
	// Step box2d world - add if box2d is enabled conditional
	float time_step = 1.0f / 60.0f;
	int32 velocity_iterations = 10;
	int32 position_iterations = 5;
	world_->Step(time_step, velocity_iterations, position_iterations);

	// Do game logic for gameobjects
	UpdateGameObjects(ticks_, frame_counter_);

	// Do input
	const abfw::SonyController* controller = controller_manager_.GetController(0);
	if (controller) // if controller isn't null
	{
		if (controller->buttons_down() & ABFW_SONY_CTRL_START)
		{
			return MENU; // go to menu if start is pressed.
		}
		else // do input loop for state if we aren't returning to menustate
			InputLoop(controller);
	}

	return LEVEL_1; // else continue with Level_1
}

void LevelState::Render(const float frame_rate_, abfw::Font& font_, abfw::SpriteRenderer* sprite_renderer_)
{
	for (auto tile : Low_Layer_)
	{
		sprite_renderer_->DrawSprite(tile);
	}

	for (auto tile : Mid_Layer_)
	{
		sprite_renderer_->DrawSprite(tile);
	}

	for (auto pickup : pickups_)
	{
		sprite_renderer_->DrawSprite(pickup);
	}

	sprite_renderer_->DrawSprite(player_);
	sprite_renderer_->DrawSprite(arrow_);

	for (auto tile : High_Layer_)
	{
		sprite_renderer_->DrawSprite(tile);
	}

	// UI
	font_.RenderText(sprite_renderer_, abfw::Vector3(10.0f, 5.0f, -0.9f), 1.0f, 0xff00ff00, abfw::TJ_LEFT, "Galaxea");
	font_.RenderText(sprite_renderer_, abfw::Vector3(815.0f, 40.0f, -0.9f), 1.0f, 0xff00ffff, abfw::TJ_LEFT, "health  : %.0f", player_.health());	//display player health
	font_.RenderText(sprite_renderer_, abfw::Vector3(815.0f, 5.0f, -0.9f), 1.0f, 0xff00ffff, abfw::TJ_LEFT, "Score : %.0f", score_);//display player score
	font_.RenderText(sprite_renderer_, abfw::Vector3(850.0f, 510.0f, -0.9f), 1.0f, 0xff00ffff, abfw::TJ_LEFT, "FPS: %.1f", frame_rate_);
}

void LevelState::LoadAssets()
{
	LoadTextures();
	LoadSounds();
	LoadMap();
}

void LevelState::ConstructBackgroundVectorsFromMap()
{

}

void LevelState::ConstructPhysicsBodiesFromMap()
{

}

void* LevelState::loadFile(const char * filename, bool appendNull) {

	FILE* f = fopen(filename, "r");
	if (!f) {
		return 0;
	}

	fseek(f, 0, SEEK_END);
	auto length = ftell(f) + appendNull;
	fseek(f, 0, SEEK_SET);

	void* buffer = malloc(length);
	fread(buffer, length, 1, f);
	fclose(f);

	if (appendNull) {
		((char*)buffer)[length - 1] = 0;
	}

	return buffer;
}