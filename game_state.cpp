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
	world_->SetContactListener(&contact_listener_);

	// Do any other init here
	// Init Objects

	background_.set_height(platform_.height());
	background_.set_width(platform_.width());
	background_.set_position(abfw::Vector3(platform_.width()*0.5f, platform_.height()*0.5f, -1.0f));

	player_.set_texture(player_tex);

	// spawn spikes, platforms, etc

	score_ = 0;
	game_over_ = false;
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

	// Do game logic for gameobjects
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
	background_.set_texture(application_->LoadTextureFromPNG("Level_BG.png"));

	// state-level textures 
	player_tex = application_->LoadTextureFromPNG("Character.png");
	rotPlayerTex = application_->LoadTextureFromPNG("Character_rotate.png");
	redPUTex = application_->LoadTextureFromPNG("Red.png");
	bluePUTex = application_->LoadTextureFromPNG("Blue.png");
	platformTex = application_->LoadTextureFromPNG("Platform_Panel.png");
	plantWallTex = application_->LoadTextureFromPNG("Plant_Wall.png");
	plantBlockTex = application_->LoadTextureFromPNG("Plant_Block.png");
	rotPlantBlockTex = application_->LoadTextureFromPNG("Plant_Block_rot.png");
}

void GameState::LoadSounds()
{
	// Load audio using audio_manager_->LoadSample("sfxname.wav", platform_)

}

void GameState::InputLoop(const abfw::SonyController* controller)
{
	player_.Player_Input(controller);
}

void GameState::UpdateGameObjects(const float& ticks_, const int& frame_counter_)
{
	// Any per-frame or per-tick updating for game objects should be done here
	player_.Update(ticks_, game_over_);
}

void GameState::SpawnSpike(b2Vec3 position_, b2Vec2 dimensions_)
{
	float x_pos_ = position_.x;
	float y_pos_ = position_.y;
	float width_ = dimensions_.x;
	float height_ = dimensions_.y;

	GameObject spike_ = GameObject();

	// Define and add box2d body
	b2BodyDef body_;
	body_.type = b2_staticBody;
	body_.position = b2Vec2(GFX_BOX2D_POS_X(x_pos_), GFX_BOX2D_POS_Y(y_pos_));
	spike_.AddBody(world_, body_); // this also changes this object to use box2d physics in all its functions

	// Define and add box2d fixture
	b2FixtureDef fixture_;
	b2PolygonShape shape_;
	shape_.SetAsBox(GFX_BOX2D_SIZE(dimensions_.x), GFX_BOX2D_SIZE(dimensions_.y));
	fixture_.shape = &shape_;
	fixture_.density = 0.0f;
	spike_.AddFixture(fixture_);

	// Initialize all the gameobject related things for this spike
	spike_.InitSprite(dimensions_.x, dimensions_.y, abfw::Vector3(x_pos_, y_pos_, 0.0f), spike_texture); // init sprite properties
	spike_.set_type(GameObject::SPIKE);
	spike_.set_visibility(true);			// Makes projectile visible

	// adds this spike to the spikes vector
	spikes_.push_back(spike_);

}