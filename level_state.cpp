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
	current_state_(state),
	pause_selection_(RESUME)
{
	world_ = new b2World(b2Vec2_zero);
	world_->SetAllowSleeping(true);
	world_->SetContinuousPhysics(true);
	world_->SetContactListener(&contact_listener_);

	pause_buttons_[0] = Button(application_->LoadTextureFromPNG("pause_resume.png"), application_->LoadTextureFromPNG("pause_resume_highlighted.png"));
	pause_buttons_[1] = Button(application_->LoadTextureFromPNG("pause_restart.png"), application_->LoadTextureFromPNG("pause_restart_highlighted.png"));
	pause_buttons_[2] = Button(application_->LoadTextureFromPNG("pause_quit.png"), application_->LoadTextureFromPNG("pause_quit_highlighted.png"));

	for (int buttonindex = 0; buttonindex < pause_buttons_.size(); buttonindex++)
	{
		pause_buttons_[buttonindex].set_position(470.0f, (platform_.height() / pause_buttons_.size() + 2) * buttonindex + 1, 0.0f);
	}
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

	// Pickup Textures
	DeleteNull(red_pickup_texture_);
	DeleteNull(blue_pickup_texture_);
	DeleteNull(yellow_pickup_texture_);
	DeleteNull(green_pickup_texture_);

	// Plant Textures
	DeleteNull(plant_wall_texture_);
	DeleteNull(plant_block_texture_);

	// Spike Texture
	DeleteNull(spike_texture_);

	// Player Textures
	DeleteNull(playerArrow);
	DeleteNull(playerTex);
	DeleteNull(rotPlayerTex);
	DeleteNull(playerIdle);
	DeleteNull(rotPlayerIdle);
	DeleteNull(playerDeath);
	DeleteNull(rotPlayerDeath);
	DeleteNull(playerJump);
	DeleteNull(rotPlayerJump);

	// Enemy Textures
	DeleteNull(enemyMove);
	DeleteNull(enemyDeath);
	DeleteNull(enemyAttack);
	DeleteNull(enemyHit);
	DeleteNull(enemyIdle);

	// delete all collision layer bodies
	for (int collisiontileindex = 0 ; collisiontileindex < level_map_.collision_layer.size(); collisiontileindex++)
	{
		DeleteNull(level_map_.collision_layer[collisiontileindex]);
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
			return current_state_;
		}
		else if (!paused_) // do input loop for state if we aren't returning to menustate
		{
			return InputLoop(controller);
		}
	}

	return current_state_;
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

void LevelState::Pause(bool _state)
{
	paused_ = _state;
}

APPSTATE LevelState::PauseInputLoop(const abfw::SonyController* controller)
{
	switch(pause_selection_)
	{
	case RESUME:
		if (controller->buttons_pressed() & ABFW_SONY_CTRL_CROSS)
		{
			paused_ = false;
		}
		if (controller->buttons_pressed() & ABFW_SONY_CTRL_DOWN)
		{
			pause_selection_ = RESTART;
			pause_buttons_[1].Select(true);
			pause_buttons_[0].Select(false);
		}
		break;
	case RESTART:
		if (controller->buttons_pressed() & ABFW_SONY_CTRL_UP)
		{
			pause_selection_ = RESUME;
			pause_buttons_[0].Select(true);
			pause_buttons_[1].Select(false);
		}
		if (controller->buttons_pressed() & ABFW_SONY_CTRL_CROSS)
		{
			Restart();
		}
		if (controller->buttons_pressed() & ABFW_SONY_CTRL_DOWN)
		{
			pause_selection_ = QUIT;
			pause_buttons_[2].Select(true);
			pause_buttons_[1].Select(false);
		}
		break;
	case QUIT:
		if (controller->buttons_pressed() & ABFW_SONY_CTRL_UP)
		{
			pause_selection_ = RESTART;
			pause_buttons_[1].Select(true);
			pause_buttons_[2].Select(false);
		}
		if (controller->buttons_pressed() & ABFW_SONY_CTRL_CROSS)
		{
			return MENU;
		}
		break;
	}
	return current_state_;
}

void LevelState::LoadMap(const char* map_filename)
{
	NLTmxMap* map_ = NLLoadTmxMap(map_filename);

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
		else if (layer->name == "Collision") { layer_tile_size = 128; layer_type_ = COLLISION; }

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

					// flags type as COLLISIONTILE
					tile->setType(GameObject::COLLISIONTILE);

					float x_pos = layer_tile_size * x;
					float y_pos = layer_tile_size * y;

					// Define and add box2d body
					b2BodyDef body;
					body.type = b2_staticBody;
					body.position = b2Vec2(GFX_BOX2D_POS_X(x_pos), GFX_BOX2D_POS_Y(y_pos));
					tile->AddBody(world_, body);

					// Instantiate fixture and its shape
					b2FixtureDef fixture;
					b2PolygonShape shape;

					// change fixture shape depending on the shapetype of tile
					switch(tile->shapetype_)
					{
					case CollisionTile::BOX:
						// normal square box
						shape.SetAsBox(GFX_BOX2D_SIZE(layer_tile_size), GFX_BOX2D_SIZE(layer_tile_size));
						break;
					case CollisionTile::DIAGONAL:
						// create a diamond (box rotated 45 degrees about its centre)
						shape.SetAsBox(GFX_BOX2D_SIZE(layer_tile_size), GFX_BOX2D_SIZE(layer_tile_size), b2Vec2(0.0f, 0.0f), 45.0f);
						break;
					}
					
					// complete the shape definition and add fixture
					fixture.shape = &shape;
					fixture.density = 0.0f;
					tile->AddFixture(fixture);

					// push completed tile to collision layer
					level_map_.collision_layer.push_back(tile);
				}
				
				dataindex++;
			}
		}
	}
}

void LevelState::SpawnSpike(b2Vec2 _position, b2Vec2 _dimensions)
{
	float x_pos = _position.x;
	float y_pos = _position.y;
	float width = _dimensions.x;
	float height = _dimensions.y;

	GameObject spike = GameObject();

	// Define and add box2d body
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position = b2Vec2(GFX_BOX2D_POS_X(x_pos), GFX_BOX2D_POS_Y(y_pos));
	spike.AddBody(world_, body); // this also changes this object to use box2d physics in all its functions

	// Define and add box2d fixture
	b2FixtureDef fixture;
	b2PolygonShape shape;
	shape.SetAsBox(GFX_BOX2D_SIZE(width/2.0f), GFX_BOX2D_SIZE(height/2.0f));
	fixture.shape = &shape;
	fixture.density = 0.0f;
	spike.AddFixture(fixture);

	// Initialize all the gameobject related things for this spike
	spike.InitSprite(width, height, abfw::Vector3(x_pos, y_pos, 0.0f), spike_texture_); // init sprite properties
	spike.setType(GameObject::SPIKE);
	spike.set_visibility(true);			// Makes projectile visible

	// adds this spike to the spikes vector
	spikes_.push_back(spike);

}

void LevelState::SpawnPickup(b2Vec2 _spawn_position, b2Vec2 _dimensions, PickUp::PICKUPTYPE _pickup_type)
{
	float x_pos = _spawn_position.x;
	float y_pos = _spawn_position.y;

	float width = _dimensions.x;
	float height = _dimensions.y;

	PickUp pickup = PickUp();

	// Define and add box2d body
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position = b2Vec2(GFX_BOX2D_POS_X(x_pos), GFX_BOX2D_POS_Y(y_pos));
	body.fixedRotation = true;
	pickup.AddBody(world_, body); // this also changes this object to use box2d physics in all its functions

	// Define and add box2d fixture
	b2FixtureDef fixture;
	b2PolygonShape shape;
	shape.SetAsBox(GFX_BOX2D_SIZE(width/2.0f), GFX_BOX2D_SIZE(height/2.0f));
	fixture.shape = &shape;
	fixture.density = 0.0f;
	pickup.AddFixture(fixture);

	abfw::Texture* texture;
	
	switch(_pickup_type)
	{
	case PickUp::RED:
		texture = red_pickup_texture_;
		break;
	case PickUp::BLUE:
		texture = blue_pickup_texture_;
		break;
	case PickUp::YELLOW:
		texture = yellow_pickup_texture_;
		break;
	case PickUp::GREEN:
		texture = green_pickup_texture_;
		break;
	}

	// Initialize all the gameobject related things for this pickup
	pickup.InitSprite(width, height, abfw::Vector3(x_pos, y_pos, 0.0f), texture); // init sprite properties
	pickup.setType(GameObject::PICKUP);
	pickup.set_visibility(true);

	pickups_.push_back(pickup);
}

void LevelState::Destroy(GameObject &object)
{
	if(object.destroyed == false)
	{
		object.DestroyBody();
		object.destroyed = true;
	}
}