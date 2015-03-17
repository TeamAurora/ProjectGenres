#include "level_state.h"
#include <graphics/sprite_renderer.h>
#include <audio/vita/audio_manager_vita.h>
#include <graphics/texture.h>
#include <iostream>
#include "game_application.h"
#include "box2d_helpers.h"

LevelState::LevelState()
{
}


LevelState::~LevelState()
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