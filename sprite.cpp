#include "sprite.h"

Sprite::Sprite() :
	frame_duration_(0.0f),
	frame_count_(0),
	looping_(false),
	sheettype_(SCROLL_X),
	x_frame_count_(0),
	y_frame_count_(0),
	animation_time_(0.0f),
	current_frame_(1)
{
}

const bool Sprite::Animate(float ticks, bool facing)
{
	if((current_frame_ >= frame_count_) && (looping_ == false)) // if the current animation frame is higher than the number of frames in the animation and looping is false
	{
		return true; // returns true to signify that the animation has completed
	}
	animation_time_ += ticks;
	if(animation_time_ > frame_duration_)
	{
		animation_time_ -= frame_duration_;
		current_frame_++; // adds 1 to current animation frame count

		if(facing == true)//check facing
		{
			switch(sheettype_)
			{
				case SCROLL_Y:
					uv_position_.y -= uv_height_;							// scroll down 1 frame on y axis
					if(uv_position_.y > (frame_count_ - 1) * uv_height_)	// if moved off bottom edge of sprite
					{
						uv_position_.y = 0;									// moves back to start of column
					}
					break;
				case SCROLL_X:
					uv_position_.x += uv_width_;							// scroll across 1 frame on x axis
					if(uv_position_.x > (frame_count_ - 1) * uv_width_)		// if moved off right edge of sprite
					{
						uv_position_.x = 0;									// moves back to start of row
					}
					break;
				case SCROLL_XY:
					uv_position_.x += uv_width_;								// scroll across 1 frame on x axis
					if(uv_position_.x > (x_frame_count_ - 1) * uv_width_)		// if moved off right edge of sprite
					{
						uv_position_.x = 0;										// moves back to start of row
						uv_position_.y += uv_height_;							// scrolls down 1 frame on y axis
						if(uv_position_.y > (y_frame_count_ - 1) * uv_height_)	// if moved off bottom edge of sprite
						{
							uv_position_.y = 0;									// moves back to start of column
						}
					}
					break;
				case SCROLL_YX:
					uv_position_.y += uv_height_;								// scrolls down 1 frame on y axis
					if(uv_position_.y > (y_frame_count_ - 1) * uv_height_)		// if moved off bottom edge of sprite
					{
						uv_position_.y = 0;										// moves back to start of column
						uv_position_.x += uv_width_;								// scroll across 1 frame on x axis
						if(uv_position_.x > (x_frame_count_ - 1) * uv_width_)	// if moved off right edge of sprite
						{
							uv_position_.x = 0;									// moves back to start of row
						}
					}
					break;
			}
		}
		else//same as above but facing the opposite direction
		{
			switch(sheettype_)
			{
				case SCROLL_Y:
					uv_position_.y += uv_height_;							// scroll down 1 frame on y axis
					if(uv_position_.y > (frame_count_ - 1) * -uv_height_)	// if moved off bottom edge of sprite
					{
						uv_position_.y = 0;									// moves back to start of column
					}
					break;
				case SCROLL_X:
					uv_position_.x -= uv_width_;							// scroll across 1 frame on x axis
					if(uv_position_.x > (frame_count_ - 1) * -uv_width_)		// if moved off right edge of sprite
					{
						uv_position_.x = 0;									// moves back to start of row
					}
					break;
				case SCROLL_XY:
					uv_position_.x -= uv_width_;								// scroll across 1 frame on x axis
					if(uv_position_.x > (x_frame_count_ - 1) * -uv_width_)		// if moved off right edge of sprite
					{
						uv_position_.x = -uv_width_;							// moves back to start of row
						uv_position_.y += uv_height_;							// scrolls down 1 frame on y axis
						if(uv_position_.y > (y_frame_count_ - 1) * uv_height_)	// if moved off bottom edge of sprite
						{
							uv_position_.y = 0.0;								// moves back to start of column
						}
					}
					break;
				case SCROLL_YX:
					uv_position_.y -= uv_height_;								// scrolls down 1 frame on y axis
					if(uv_position_.y > (y_frame_count_ - 1) * -uv_height_)		// if moved off bottom edge of sprite
					{
						uv_position_.y = -uv_height_;							// moves back to start of column
						uv_position_.x += uv_width_;							// scroll across 1 frame on x axis
						if(uv_position_.x > (x_frame_count_ - 1) * uv_width_)	// if moved off right edge of sprite
						{
							uv_position_.x = 0;									// moves back to start of row
						}
					}
					break;
			}
		}
	}
	return false; // animation has not completed thus return false
}

void Sprite::ResetAnimation()
{
	animation_time_ = 0.0f;
	current_frame_ = 1;
}

void Sprite::InitSprite(const float width, const float height, const abfw::Vector3 position, const abfw::Texture* texture)
{
	set_width(width);
	set_height(height);
	set_position(position);
	set_texture(texture);
}

void Sprite::InitSpriteAnimation(const float frame_duration, const int frame_count, const bool looping, const SHEETTYPE sheettype, const int x_frame_count, const int y_frame_count)
{
	frame_duration_ = frame_duration; // in seconds
	frame_count_ = frame_count;
	looping_ = looping;
	sheettype_ = sheettype;
	x_frame_count_ = x_frame_count;
	y_frame_count_ = y_frame_count;
	animation_time_ = 0.0f; // animation time begins at 0.0f
	current_frame_ = 1; // animation begins at frame 1
}