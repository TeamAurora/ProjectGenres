#pragma once
#include <graphics/sprite.h>

class Sprite : public abfw::Sprite
{
public:
	Sprite();
	enum SHEETTYPE { SCROLL_Y, SCROLL_X, SCROLL_XY, SCROLL_YX }; // scrolls down y, scrolls across x, scrolls x then y, scrolls y then x
	
	const bool Animate(float ticks, bool right); // returns whether or not animation has finished
	void ResetAnimation();
	void InitSprite(const float width, const float height, const abfw::Vector3 position, const abfw::Texture* texture);
	void InitSpriteAnimation(const float frame_duration, const int frame_count, const bool looping, const SHEETTYPE sheettype, const int x_frame_count, const int y_frame_count); // x and y frame count required for XY and YX spritesheet types only
	

private:
	SHEETTYPE sheettype_; // type of scrolling to use on the spritesheet
	float frame_duration_; // animation time per frame in seconds
	float animation_time_; // current progress through animation in seconds
	bool looping_; // if true - the animation loops, if false - the animation will not loop and will stay at last frame when the animation has finished
	int current_frame_; // tracks current frame number
	int frame_count_; // total number of frames in the animation
	int x_frame_count_; // used to track amount of frames in a row for animations that span more than 1 row
	int y_frame_count_; // used to track amount of frames in a column for animations that span more than 1 column
};