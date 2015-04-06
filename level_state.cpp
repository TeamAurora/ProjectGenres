#include "level_state.h"
#include <graphics/sprite_renderer.h>
#include <audio/vita/audio_manager_vita.h>
#include <graphics/texture.h>
#include <iostream>
#include "game_application.h"
#include "box2d_helpers.h"
#include <memory>

LevelState::LevelState(abfw::Platform& platform, const GameApplication* application, abfw::AudioManager* audio_manager, APPSTATE state) :
	AppState(platform, application, audio_manager),
	paused_(false),
	current_state_(state)
{
	world_ = new b2World(b2Vec2_zero);
	world_->SetAllowSleeping(true);
	world_->SetContinuousPhysics(true);
	world_->SetContactListener(&contact_listener_);
}


LevelState::~LevelState()
{
}

void LevelState::InitializeState()
{

}

void LevelState::TerminateState()
{
	for (int i = 0 ; i < level_map_.collision_layer.size(); i++)
	{
		auto body = level_map_.collision_layer[i]->body_;
		world_->DestroyBody(body);
	}
	DeleteNull(world_);
}

APPSTATE LevelState::Update(const float& ticks_, const int& frame_counter_, const abfw::SonyControllerInputManager& controller_manager_)
{
	if (!paused_)
	{
		// Step box2d world - add if box2d is enabled conditional
		float time_step = 1.0f / 60.0f;
		int32 velocity_iterations = 10;
		int32 position_iterations = 5;
		world_->Step(time_step, velocity_iterations, position_iterations);

		// Do game logic for gameobjects
		UpdateGameObjects(ticks_, frame_counter_);
	}

	// Do input
	const abfw::SonyController* controller = controller_manager_.GetController(0);
	if (controller) // if controller exists
	{
		if(paused_)
		{
			return PauseInputLoop(controller);
		}
		else if (controller->buttons_pressed() & ABFW_SONY_CTRL_START)
		{
			Pause(true);
		}
		else if (!paused_) // do input loop for state if we aren't returning to menustate
		{
			return InputLoop(controller);
		}
	}
}

void LevelState::Render(const float frame_rate_, abfw::Font& font_, abfw::SpriteRenderer* sprite_renderer_)
{
	for (int tile = 0 ; tile < level_map_.low_layer.size(); tile++)
	{
		sprite_renderer_->DrawSprite(level_map_.low_layer[tile]);
	}

	for (int tile = 0 ; tile < level_map_.mid_layer.size(); tile++)
	{
		sprite_renderer_->DrawSprite(level_map_.mid_layer[tile]);
	}

	for (int pickup = 0; pickup < pickups_.size(); pickup++)
	{
		sprite_renderer_->DrawSprite(pickups_[pickup]);
	}

	sprite_renderer_->DrawSprite(player_);
	sprite_renderer_->DrawSprite(arrow_);

	for (int tile = 0 ; tile < level_map_.high_layer.size(); tile++)
	{
		sprite_renderer_->DrawSprite(level_map_.high_layer[tile]);
	}

	if (paused_)
	{
		sprite_renderer_->DrawSprite(pause_background_);
		for ( int button = 0 ; button < pause_buttons_.size(); button++)
		{
			sprite_renderer_->DrawSprite(pause_buttons_[button]);
		}
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
}

void LevelState::LoadMap(string map_filename)
{
	NLTmxMap* map_ = NLLoadTmxMap(map_filename.c_str());

	// create vector of tile textures which will be used to create sprites
	std::vector<abfw::Texture*> tiles;
	//std::vector<bool> tiles_used(map_->totalTileCount, false);
	tiles.reserve(map_->totalTileCount); // reserve enough space (prevent constant resizing every push_back)

	// load all textures
	for (int tilesetindex = 0 ; tilesetindex < map_->tilesets.size(); tilesetindex++)
	{
		auto tileset = map_->tilesets[tilesetindex];
		if (tileset->name == "Collision") // don't bother loading collision tilesets textures
			continue;

		for (int tileindex = 0 ; tileindex < tileset->tiles.size(); tileindex++)
		{
			auto tile = tileset->tiles[tileindex];
			auto tiletexture = application_->LoadTextureFromPNG(tile->filename.c_str());
			tiles.push_back(tiletexture);
		}
	}

	for (int layerindex = 0 ; layerindex < map_->layers.size() ; layerindex++)
	{
		auto layer = map_->layers[layerindex];
		enum LAYER { LOW = 1, MID = 2, HIGH = 3, COLLISION = 4 };
		LAYER layer_type_;
		int layer_tile_size;

		if (layer->name == "Low128") { layer_tile_size = 128; layer_type_ = LOW; }
		else if (layer->name == "Low256") { layer_tile_size = 256; layer_type_ = LOW; }
		else if (layer->name == "Low512") { layer_tile_size = 512; layer_type_ = LOW; }
		else if (layer->name == "Mid128") { layer_tile_size = 128; layer_type_ = MID; }
		else if (layer->name == "Mid256") { layer_tile_size = 256; layer_type_ = MID; }
		else if (layer->name == "Mid512") { layer_tile_size = 512; layer_type_ = MID; }
		else if (layer->name == "High128") { layer_tile_size = 128; layer_type_ = HIGH; }
		else if (layer->name == "High256") { layer_tile_size = 256; layer_type_ = HIGH; }
		else if (layer->name == "High512") { layer_tile_size = 512; layer_type_ = HIGH; }
		else if (layer->name == "Collision") { layer_type_ = COLLISION; }

		auto index = layer->data.begin();
		
		// This should really use layer dimensions but for our purposes
		// all layers are the same size as the map so we dont store layer dimensions
		// and instead assume that they are equivalent of the map dimensions
		for (int y = 0; y < map_->height; y++) 
		{
			for (int x = 0; x < map_->width; x++)
			{
				if (*index == 0) // if 0, this is an empty tile so skip this iteration
					continue;

				if (layer_type_ == LOW || MID || HIGH) // if it's one of the render layers
				{
					// Create sprite with correct texture and position it
					Sprite sprite_;
					switch (layer_tile_size)
					{
					case 128:
						sprite_.InitSprite(128, 128, abfw::Vector3(128 * x, 128 * y, 0.0f), tiles[*index]);
						break;
					case 256:
						sprite_.InitSprite(256, 256, abfw::Vector3(128 * x + 64, 128 * y + 64, 0.0f), tiles[*index]);
						break;
					case 512:
						sprite_.InitSprite(512, 512, abfw::Vector3(128 * x + 192, 128 * y + 192, 0.0f), tiles[*index]);
						break;
					}

					switch (layer_type_)
					{
					case LOW:
						level_map_.low_layer.push_back(sprite_);
						break;
					case MID:
						level_map_.mid_layer.push_back(sprite_);
						break;
					case HIGH:
						level_map_.high_layer.push_back(sprite_);
						break;
					}
				}

				if (layer_type_ == COLLISION)
				{

				}

				// TODO collision layer
				
				index++;
			}
		}
	}
}