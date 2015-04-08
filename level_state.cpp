#include "level_state.h"
#include <graphics/sprite_renderer.h>
#include <audio/vita/audio_manager_vita.h>
#include <graphics/texture.h>
#include <iostream>
#include "game_application.h"
#include "box2d_helpers.h"

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
	LoadAssets();

	score_ = 0;
	gameOver_ = false;
	reloadTime = 0;
}

void LevelState::TerminateState()
{
	// cleanup all tile textures
	for(int texturesindex = 0; texturesindex < level_map_.textures.size(); texturesindex++)
	{
		DeleteNull(level_map_.textures[texturesindex]);
	}

	// delete all collision layer bodies
	for (int collisiontileindex = 0 ; collisiontileindex < level_map_.collision_layer.size(); collisiontileindex++)
	{
		level_map_.collision_layer[collisiontileindex]->DestroyBody();
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

	if(player_.deadAnim == false)
	{
		sprite_renderer_->DrawSprite(player_);
		sprite_renderer_->DrawSprite(arrow_);
		if(blade_.destroyed == false)
		{
			sprite_renderer_->DrawSprite(blade_);
		}
	}

	for(int enemyindex = 0; enemyindex < enemies_.size(); enemyindex++)
	{
		if(enemies_[enemyindex].deadAnim == false)
		{
			sprite_renderer_->DrawSprite(enemies_[enemyindex]);
		}
	}

	for(int pickupindex = 0; pickupindex < pickups_.size(); pickupindex++)
	{
		if(pickups_[pickupindex].dead == false)
		{
			sprite_renderer_->DrawSprite(pickups_[pickupindex]);
		}
	}

	for(int bulletindex = 0; bulletindex < bullets_.size(); bulletindex++)
	{
		if(bullets_[bulletindex].dead == false)
		{
			sprite_renderer_->DrawSprite(bullets_[bulletindex]);
		}
	}

	for (int tile = 0 ; tile < level_map_.high_layer.size(); tile++)
	{
		sprite_renderer_->DrawSprite(level_map_.high_layer[tile]);
	}

	// UI
	font_.RenderText(sprite_renderer_, abfw::Vector3(10.0f, 5.0f, -0.9f), 1.0f, 0xff00ff00, abfw::TJ_LEFT, "Galaxea");
	font_.RenderText(sprite_renderer_, abfw::Vector3(815.0f, 40.0f, -0.9f), 1.0f, 0xff00ffff, abfw::TJ_LEFT, "health  : %.0f", player_.health());	//display player health
	font_.RenderText(sprite_renderer_, abfw::Vector3(815.0f, 5.0f, -0.9f), 1.0f, 0xff00ffff, abfw::TJ_LEFT, "Score : %.0f", score_);//display player score
	font_.RenderText(sprite_renderer_, abfw::Vector3(850.0f, 510.0f, -0.9f), 1.0f, 0xff00ffff, abfw::TJ_LEFT, "FPS: %.1f", frame_rate_);

	if (paused_)
	{
		sprite_renderer_->DrawSprite(pause_background_);
		for ( int button = 0 ; button < pause_buttons_.size(); button++)
		{
			sprite_renderer_->DrawSprite(pause_buttons_[button]);
		}
	}
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
	//std::vector<bool> tiles_used(map_->totalTileCount, false);
	level_map_.textures.reserve(map_->totalTileCount); // reserve enough space (prevent constant resizing every push_back)

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
			level_map_.textures.push_back(tiletexture);
		}
	}

	// load all layers
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

		// Begin iterating over layer data
		auto dataindex = layer->data.begin();
		
		// iterate over map in x then y order
		for (int y = 0; y < map_->height; y++) 
		{
			for (int x = 0; x < map_->width; x++)
			{
				if (*dataindex == 0) // if data == 0, this is an empty tile so skip this iteration
					continue;

				if (layer_type_ == LOW || MID || HIGH) // if we're doing a render layer
				{
					// Create sprite with correct texture and position it
					Sprite sprite_;
					switch (layer_tile_size)
					{
					case 128:
						sprite_.InitSprite(128, 128, abfw::Vector3(128 * x, 128 * y, 0.0f), level_map_.textures[*dataindex - 1]);
						break;
					case 256:
						sprite_.InitSprite(256, 256, abfw::Vector3(128 * x + 64, 128 * y + 64, 0.0f), level_map_.textures[*dataindex - 1]);
						break;
					case 512:
						sprite_.InitSprite(512, 512, abfw::Vector3(128 * x + 192, 128 * y + 192, 0.0f), level_map_.textures[*dataindex - 1]);
						break;
					}

					// Push sprite to appropriate layer
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
					CollisionTile* tile;

					b2BodyDef body;


					tile->AddBody(world_, body);
					// Assumption that the data for the collision tiles is correct (collision tileset is the first loaded tileset and in right order)
					switch (*dataindex)
					{
					case 1:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->edges_.DOWN = true;
						tile->edges_.UP = true;
						tile->edges_.LEFT = true;
						tile->edges_.RIGHT = true;
						break;
					case 2:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->edges_.DOWN = true;
						tile->edges_.LEFT = true;
						tile->edges_.RIGHT = true;
						break;
					case 3:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->edges_.DOWN = true;
						tile->edges_.UP = true;
						tile->edges_.RIGHT = true;
						break;
					case 4:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->edges_.UP = true;
						tile->edges_.LEFT = true;
						tile->edges_.RIGHT = true;
						break;
					case 5:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->edges_.DOWN = true;
						tile->edges_.UP = true;
						tile->edges_.LEFT = true;
						break;
					case 6:
						tile = new CollisionTile(CollisionTile::DIAGONAL);
						tile->diagonal_ = CollisionTile::BOTTOMRIGHT;
						break;
					case 7:
						tile = new CollisionTile(CollisionTile::DIAGONAL);
						tile->diagonal_ = CollisionTile::BOTTOMLEFT;
						break;
					case 8:
						tile = new CollisionTile(CollisionTile::DIAGONAL);
						tile->diagonal_ = CollisionTile::TOPRIGHT;
						break;
					case 9:
						tile = new CollisionTile(CollisionTile::DIAGONAL);
						tile->diagonal_ = CollisionTile::TOPLEFT;
						break;
					case 10:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->edges_.LEFT = true;
						tile->edges_.RIGHT = true;
						break;
					case 11:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->edges_.DOWN = true;
						tile->edges_.UP = true;
						break;
					case 12:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->edges_.DOWN = true;
						tile->edges_.RIGHT = true;
						break;
					case 13:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->edges_.UP = true;
						tile->edges_.RIGHT = true;
						break;
					case 14:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->edges_.UP = true;
						tile->edges_.LEFT = true;
						break;
					case 15:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->edges_.DOWN = true;
						tile->edges_.LEFT = true;
						break;
					case 16:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->edges_.UP = true;
						break;
					case 17:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->edges_.LEFT = true;
						break;
					case 18:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->edges_.DOWN = true;
						break;
					case 19:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->edges_.RIGHT = true;
						break;
					}
				}
				
				dataindex++;
			}
		}
	}
}

void LevelState::SpawnSpike(b2Vec3 position_, b2Vec2 dimensions_)
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
	spike_.setType(GameObject::SPIKE);
	spike_.set_visibility(true);			// Makes projectile visible

	// adds this spike to the spikes vector
	spikes_.push_back(spike_);

}

void LevelState::Destroy(GameObject &object)
{
	if(object.destroyed == false)
	{
		world_->DestroyBody(object.body_);
		object.body_ = NULL;
		object.destroyed = true;
	}
}