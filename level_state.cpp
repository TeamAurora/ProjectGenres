#include "level_state.h"
#include <graphics/sprite_renderer.h>
#include <audio/vita/audio_manager_vita.h>
#include <system/vita/file_vita.h>
#include <graphics/texture.h>
#include <iostream>
#include "game_application.h"
#include "box2d_helpers.h"
#include "NLTmxMap.h"

LevelState::LevelState(abfw::Platform& platform, const GameApplication* application, abfw::AudioManager* audio_manager, APPSTATE state) :
	AppState(platform, application, audio_manager),
	paused_(false),
	current_state_(state),
	pause_selection_(RESUME),
	score_(0),
	max_score_(100),
	gameOver_(false),
	reloadTime(0)
{
	world_ = new b2World(b2Vec2_zero);
	world_->SetAllowSleeping(true);
	world_->SetContinuousPhysics(true);
	world_->SetContactListener(&contact_listener_);

	pause_buttons_[0] = new Button(application_->LoadTextureFromPNG("pause_resume.png"), application_->LoadTextureFromPNG("pause_resume_highlighted.png"));
	pause_buttons_[1] = new Button(application_->LoadTextureFromPNG("pause_restart.png"), application_->LoadTextureFromPNG("pause_restart_highlighted.png"));
	pause_buttons_[2] = new Button(application_->LoadTextureFromPNG("pause_quit.png"), application_->LoadTextureFromPNG("pause_quit_highlighted.png"));

	pause_background_ = Sprite();
	pause_background_texture_ = application_->LoadTextureFromPNG("pause_background.png");
	pause_background_.InitSprite(platform_.width(), platform_.height(), abfw::Vector3(platform_.width() / 2.0f, platform_.height() / 2.0f, 0.0f), pause_background_texture_);

	for (int buttonindex = 0; buttonindex < pause_buttons_.size(); buttonindex++)
	{
		pause_buttons_[buttonindex]->set_width(256.0f);
		pause_buttons_[buttonindex]->set_height(64.0f);
		pause_buttons_[buttonindex]->set_position(470.0f, (platform_.height() / (pause_buttons_.size() + 1)) * (buttonindex + 1), 0.0f);
	}

	pause_buttons_[0]->Select(true);
}

LevelState::~LevelState()
{
}

void LevelState::InitializeState()
{
	LoadAssets();
	CreateObjects();
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
	//DeleteNull(spike_texture_);

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
	for (int collisiontileindex = 0; collisiontileindex < level_map_.collision_layer.size(); collisiontileindex++)
	{
		level_map_.collision_layer[collisiontileindex]->DestroyBody();
		DeleteNull(level_map_.collision_layer[collisiontileindex]);
	}

	for (int pickupindex = 0; pickupindex < pickups_.size(); pickupindex++)
	{
		pickups_[pickupindex].DestroyBody();
	}

	for (int enemiesindex = 0; enemiesindex < enemies_.size(); enemiesindex++)
	{
		enemies_[enemiesindex].DestroyBody();
	}

	for (int plantsindex = 0; plantsindex < plants_.size(); plantsindex++)
	{
		plants_[plantsindex].DestroyBody();
	}

	DeleteNull(pause_background_texture_);
	for (int buttonindex = 0; buttonindex < pause_buttons_.size(); buttonindex++)
	{
		DeleteNull(pause_buttons_[buttonindex]);
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

		application_->player_camera_->TendTowards(abfw::Vector2(player_.position().x - (platform_.width() / 2.0f), player_.position().y - (platform_.height() / 2.0f)), 15.0f);
		application_->player_camera_->UpdateCamera(ticks_);
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
			paused_ = true;
			return current_state_;
		}
		else if (!paused_) // do input loop for state if we aren't returning to menustate
		{
			return InputLoop(controller);
		}
	}
	else
	{
		std::cout << "Controller invalid." << endl;
		exit(-1);
	}
	// this will never be reached but it gets rid of annoying warning
	return current_state_;
}

void LevelState::Render(const float frame_rate_, abfw::Font& font_, abfw::SpriteRenderer* sprite_renderer_)
{
	// Use dynamic player camera to draw game world objects
	application_->player_camera_->SetActiveCamera();

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

	if(!enemies_.empty())
	{
		for(int enemyindex = 0; enemyindex < enemies_.size(); enemyindex++)
		{
			if(enemies_[enemyindex].deadAnim == false)
			{
				sprite_renderer_->DrawSprite(enemies_[enemyindex]);
			}
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

	// Use static camera at the origin to draw overlay
	application_->main_camera_->SetActiveCamera();

	// UI
	font_.RenderText(sprite_renderer_, abfw::Vector3(10.0f, 5.0f, 0.0f), 1.0f, 0xff00ff00, abfw::TJ_LEFT, "Galaxea");
	font_.RenderText(sprite_renderer_, abfw::Vector3(815.0f, 40.0f, 0.0f), 1.0f, 0xff00ffff, abfw::TJ_LEFT, "Health : %.0f", player_.health());	//display player health
	font_.RenderText(sprite_renderer_, abfw::Vector3(815.0f, 5.0f, 0.0f), 1.0f, 0xff00ffff, abfw::TJ_LEFT, "Score : %.0f", score_);//display player score

	if (paused_)
	{
		sprite_renderer_->DrawSprite(pause_background_);
		for ( int button = 0 ; button < pause_buttons_.size(); button++)
		{
			sprite_renderer_->DrawSprite(*pause_buttons_[button]);
		}
	}
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
			pause_buttons_[1]->Select(true);
			pause_buttons_[0]->Select(false);
		}
		break;
	case RESTART:
		if (controller->buttons_pressed() & ABFW_SONY_CTRL_UP)
		{
			pause_selection_ = RESUME;
			pause_buttons_[0]->Select(true);
			pause_buttons_[1]->Select(false);
		}
		if (controller->buttons_pressed() & ABFW_SONY_CTRL_CROSS)
		{
			Restart();
			paused_ = false;
		}
		if (controller->buttons_pressed() & ABFW_SONY_CTRL_DOWN)
		{
			pause_selection_ = QUIT;
			pause_buttons_[2]->Select(true);
			pause_buttons_[1]->Select(false);
		}
		break;
	case QUIT:
		if (controller->buttons_pressed() & ABFW_SONY_CTRL_UP)
		{
			pause_selection_ = RESTART;
			pause_buttons_[1]->Select(true);
			pause_buttons_[2]->Select(false);
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
	bool appendNull = true;

	abfw::File* file = platform_.CreateFile();
	if ( !file->Open( map_filename) ) {
		std::cout << "Map file could not be opened." << std::endl;
		exit(-1);
	}

	Int32 file_size;
	if ( !file->GetSize(file_size))
	{
		std::cout << "Map failed to return file size." << std::endl;
		exit(-1);
	}

	auto length = file_size + appendNull;
	char* buffer = new char[length];
	Int32 bytes_read;
	if( !file->Read(buffer, file_size, bytes_read))
	{
		std::cout << "Map failed to read." << std::endl;
		exit(-1);
	}

	if ( !file->Close() ) {
		std::cout << "Map file failed to close." << std::endl;
		exit(-1);
	}

	if ( appendNull ) {
		((char*)buffer)[ length-1 ] = 0;
	}

	NLTmxMap* map_ = NLLoadTmxMap((char*)buffer);

	level_map_.textures.reserve(map_->totalTileCount); // resrve vector to make enough space for all textures (prevents constant reserving)

	// fill textures array with nulls
	for(int i = 0; i < map_->totalTileCount; i++)
	{
		level_map_.textures.push_back(NULL);
	}

	std::vector<NLTmxMapTile*> tiles_;
	tiles_.reserve(map_->totalTileCount);

	// load all tiles into tiles_ vector
	for (int tilesetindex = 0 ; tilesetindex < map_->tilesets.size(); tilesetindex++)
	{
		auto tileset = map_->tilesets[tilesetindex];

		for (int tileindex = 0 ; tileindex < tileset->tiles.size(); tileindex++)
		{
			tiles_.push_back(tileset->tiles[tileindex]);
		}
	}

	// load all layers
	for (int layerindex = 0 ; layerindex < map_->layers.size() ; layerindex++)
	{
		auto layer = map_->layers[layerindex];
		enum LAYER { LOW = 1, MID, HIGH, COLLISION};
		LAYER layer_type_;

		if (layer->name == "Low_Layer") { layer_type_ = LOW; }
		else if (layer->name == "Mid_Layer") { layer_type_ = MID; }
		else if (layer->name == "High_Layer") { layer_type_ = HIGH; }
		else if (layer->name == "Collision") { layer_type_ = COLLISION; }

		// Begin iterating over layer data
		auto dataindex = layer->data.begin();
		
		// iterate over map in x then y order
		for (int y = 0; y < map_->height; y++) 
		{
			for (int x = 0; x < map_->width; x++)
			{
				if (*dataindex == 0) // if data == 0, this is an empty tile so skip this iteration
				{
					// do nothing
				}
				else if ((layer_type_ == LOW) || (layer_type_ == MID) || (layer_type_ == HIGH)) // if we're doing a render layer
				{
					// Create sprite with correct texture and position it
					Sprite sprite = Sprite();

					NLTmxMapTile* tile = tiles_[*dataindex-1];

					abfw::Texture* texture;
					if(level_map_.textures[*dataindex-1] == NULL)
					{
						level_map_.textures[*dataindex-1] = application_->LoadTextureFromPNG(tile->filename.c_str());
					}
					texture = level_map_.textures[*dataindex-1];

					sprite.set_width(tile->width);
					sprite.set_height(tile->height);
					sprite.set_texture(texture);

					abfw::Vector2 position;

					switch(tile->width)
					{
					case 128:
						position.x = x * 128;
						break;
					case 256:
						position.x = x * 128 + 64;
						break;
					case 512:
						position.x = x * 128 + 192;
						break;
					}

					switch(tile->height)
					{
					case 128:
						position.y = y * 128;
						break;
					case 256:
						position.y = y * 128 - 64;
						break;
					case 512:
						position.y = y * 128 - 192;
						break;
					}

					sprite.set_position(abfw::Vector3(position.x, position.y, 0.0f));

					// Push sprite to appropriate layer
					switch (layer_type_)
					{
					case LOW:
						level_map_.low_layer.push_back(sprite);
						break;
					case MID:
						level_map_.mid_layer.push_back(sprite);
						break;
					case HIGH:
						level_map_.high_layer.push_back(sprite);
						break;
					}
				}
				else if (layer_type_ == COLLISION)
				{
					CollisionTile* tile;
					float tile_size = 128.0f;
					float x_pos = x * tile_size;
					float y_pos = y * tile_size;
					
					// Assumption that the data for the collision tiles is correct (collision tileset is the first loaded tileset and in right order)
					switch (*dataindex)
					{
					case 1:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->harmful_ = false;
						tile->edges_.DOWN = true;
						tile->edges_.UP = true;
						tile->edges_.LEFT = true;
						tile->edges_.RIGHT = true;
						break;
					case 2:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->harmful_ = false;
						tile->edges_.DOWN = true;
						tile->edges_.LEFT = true;
						tile->edges_.RIGHT = true;
						break;
					case 3:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->harmful_ = false;
						tile->edges_.DOWN = true;
						tile->edges_.UP = true;
						tile->edges_.RIGHT = true;
						break;
					case 4:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->harmful_ = false;
						tile->edges_.UP = true;
						tile->edges_.LEFT = true;
						tile->edges_.RIGHT = true;
						break;
					case 5:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->harmful_ = false;
						tile->edges_.DOWN = true;
						tile->edges_.UP = true;
						tile->edges_.LEFT = true;
						break;
					case 6:
						tile = new CollisionTile(CollisionTile::DIAGONAL);
						tile->harmful_ = true;
						x_pos += tile_size / 2.0f;
						y_pos += tile_size / 2.0f;
						tile->diagonal_ = CollisionTile::BOTTOMRIGHT;
						break;
					case 7:
						tile = new CollisionTile(CollisionTile::DIAGONAL);
						tile->harmful_ = true;
						x_pos -= tile_size / 2.0f;
						y_pos += tile_size / 2.0f;
						tile->diagonal_ = CollisionTile::BOTTOMLEFT;
						break;
					case 8:
						tile = new CollisionTile(CollisionTile::DIAGONAL);
						tile->harmful_ = true;
						x_pos += tile_size / 2.0f;
						y_pos -= tile_size / 2.0f;
						tile->diagonal_ = CollisionTile::TOPRIGHT;
						break;
					case 9:
						tile = new CollisionTile(CollisionTile::DIAGONAL);
						tile->harmful_ = true;
						x_pos -= tile_size / 2.0f;
						y_pos -= tile_size / 2.0f;
						tile->diagonal_ = CollisionTile::TOPLEFT;
						break;
					case 10:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->harmful_ = false;
						tile->edges_.LEFT = true;
						tile->edges_.RIGHT = true;
						break;
					case 11:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->harmful_ = false;
						tile->edges_.DOWN = true;
						tile->edges_.UP = true;
						break;
					case 12:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->harmful_ = false;
						tile->edges_.DOWN = true;
						tile->edges_.RIGHT = true;
						break;
					case 13:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->harmful_ = false;
						tile->edges_.UP = true;
						tile->edges_.RIGHT = true;
						break;
					case 14:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->harmful_ = false;
						tile->edges_.UP = true;
						tile->edges_.LEFT = true;
						break;
					case 15:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->harmful_ = false;
						tile->edges_.DOWN = true;
						tile->edges_.LEFT = true;
						break;
					case 16:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->harmful_ = false;
						tile->edges_.UP = true;
						break;
					case 17:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->harmful_ = false;
						tile->edges_.LEFT = true;
						break;
					case 18:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->harmful_ = false;
						tile->edges_.DOWN = true;
						break;
					case 19:
						tile = new CollisionTile(CollisionTile::BOX);
						tile->harmful_ = false;
						tile->edges_.RIGHT = true;
						break;
					}

					// flags type as COLLISIONTILE
					tile->setType(GameObject::COLLISIONTILE);

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
						shape.SetAsBox(GFX_BOX2D_SIZE(tile_size / 2.0f) , GFX_BOX2D_SIZE(tile_size / 2.0f));
						break;
					case CollisionTile::DIAGONAL:
						// create a diamond (box rotated 45 degrees about its centre)
						shape.SetAsBox(GFX_BOX2D_SIZE(tile_size / 2.0f), GFX_BOX2D_SIZE(tile_size / 2.0f), b2Vec2(0.0f, 0.0f), 45.0f);
						break;
					}
					
					// complete the shape definition and add fixture
					fixture.shape = &shape;
					fixture.density = 0.0f;
					tile->AddFixture(fixture);

					tile->UpdatePosition();
					tile->set_height(tile_size);
					tile->set_width(tile_size);

					// push completed tile to collision layer
					level_map_.collision_layer.push_back(tile);
				}

				dataindex++;
			}
		}
	}

	// load all object groups
	for(int objectgroupindex = 0; objectgroupindex < map_->groups.size(); objectgroupindex++)
	{
		auto group = map_->groups[objectgroupindex];
		enum GROUPS { PICKUPS = 0, PLANTS };
		GROUPS objectgroup_type;

		if(group->name == "Pickups") { objectgroup_type = PICKUPS; }
		else if(group->name == "Plants") { objectgroup_type = PLANTS; }


		for(int objectsindex = 0; objectsindex < group->objects.size(); objectsindex++)
		{
			auto object = group->objects[objectsindex];

			// get associated tile for this object
			NLTmxMapTile* tile = tiles_[object->gid];

			switch(objectgroup_type)
			{
			case PICKUPS:
				PickUp::PICKUPTYPE type;

				if(tile->filename == "pickup_red.png") { type = PickUp::RED; }
				else if(tile->filename == "pickup_blue.png") { type = PickUp::BLUE; }
				else if(tile->filename == "pickup_yellow.png") { type = PickUp::YELLOW; }
				else if(tile->filename == "pickup_green.png") { type = PickUp::GREEN; }

				SpawnPickup(b2Vec2(object->x, object->y), b2Vec2(object->width, object->height), type);
				break;
			}
		}
	}

	// map 
	DeleteNull(map_);
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
	body.type = b2_dynamicBody;
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
		max_score_ += 30;
		break;
	case PickUp::BLUE:
		texture = blue_pickup_texture_;
		max_score_ += 60;
		break;
	case PickUp::YELLOW:
		texture = yellow_pickup_texture_;
		max_score_ += 90;
		break;
	case PickUp::GREEN:
		texture = green_pickup_texture_;
		break;
	}

	// Initialize all the gameobject related things for this pickup
	pickup.InitSprite(width, height, abfw::Vector3(x_pos, y_pos, 0.0f), texture); // init sprite properties
	pickup.pickup_type_ = _pickup_type;
	pickup.setType(GameObject::PICKUP);
	pickup.set_visibility(true);
	pickup.spawned = true;
	pickup.destroyed = false;
	pickup.dead = false;

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