#include "camera.h"
#include "graphics/sprite_renderer.h"
#include "maths/math_utils.h"
#include "system/platform.h"

Camera::Camera(abfw::SpriteRenderer* renderer, abfw::Platform& platform) :
	renderer_(renderer),
	platform_(platform)
{
	translation_ = abfw::Vector2(0.0f, 0.0f);
	target_ = translation_;
	rotation_ = 0.0f;
	scale_ = abfw::Vector2(1.0f, 1.0f);
	float left_edge = translation_.x * scale_.x;
	float right_edge = (translation_.x + platform_.width()) * scale_.x;
	float top_edge = translation_.y * scale_.y;
	float bottom_edge = (translation_.y + platform_.height()) * scale_.y;
		
	// construct the frustrum from parameters
	result_.OrthographicFrustumGL(left_edge, right_edge, top_edge, bottom_edge, -1.0f, 1.0f);
	changed_ = false;

	shaking_intensity_ = 1;
	shaking_remaining_time_ = 0.0f;
}


Camera::~Camera(void)
{
}

void Camera::TendTowards(abfw::Vector2 target, float velocity)
{
	target_ = target;
	velocity_ = velocity;
}

void Camera::MoveBy(abfw::Vector2 translation)
{
	abfw::Vector2 current_pos = translation_;
	translation_ = abfw::Vector2(current_pos.x + translation.x, current_pos.y + translation.y);
	changed_ = true;
}

void Camera::MoveTo(abfw::Vector2 new_pos)
{
	translation_ = new_pos;
	changed_ = true;
}

void Camera::Scale(abfw::Vector2 scalefactor)
{
	scale_ = scalefactor;
	changed_ = true;
}

/*void Camera::Rotate(float deg)
{
	rotation_ += abfw::DegToRad(deg);
	changed_ = true;
}*/

void Camera::ScreenShake(int intensity, float duration)
{
	shaking_remaining_time_ = duration;
	shaking_intensity_ = intensity;
}

void Camera::UpdateCamera(const float& ticks)
{
	if ((translation_.x != target_.x) && (translation_.y != target_.y)) // no Vector2 != operator
	{
		abfw::Vector2 direction = target_ - translation_;
		if(direction.Length() < (velocity_ * 1.5f)) // defines a cutoff distance at which the camera reaches target (else it will never be exactly equal)
		{
			translation_ = target_;
		}
		else
		{
			direction.Normalise();
			direction.x *= velocity_;
			direction.y *= velocity_;
			translation_ += direction;
		}
		changed_ = true;
	}

	if (shaking_remaining_time_ > 0.0f)
	{
		shaking_remaining_time_ -= ticks;
		MoveBy(abfw::Vector2((2 * rand() % shaking_intensity_) - shaking_intensity_, (2 * rand() % shaking_intensity_) - shaking_intensity_));
	}

	if(changed_) // only update the matrix on frames that it has changed
	{
		// construct edges of the frustrum using transforms
		float left_edge = translation_.x * scale_.x;
		float right_edge = (translation_.x + platform_.width()) * scale_.x;
		float top_edge = translation_.y * scale_.y;
		float bottom_edge = (translation_.y + platform_.height()) * scale_.y;
		
		// construct the frustrum from parameters
		result_.OrthographicFrustumGL(left_edge, right_edge, top_edge, bottom_edge, -1.0f, 1.0f);

		/*abfw::Matrix44 rotation;
		rotation.RotationZ(rotation_);
		result = result * rotation;*/

		changed_ = false;
	}
}

void Camera::SetActiveCamera()
{
	// push transformed frustrum to gfx renderer
	renderer_->set_projection_matrix(result_);
}

void Camera::ResetCamera()
{
	translation_ = abfw::Vector2(0.0f, 0.0f);
	target_ = translation_;
	rotation_ = 0.0f;
	scale_ = abfw::Vector2(1.0f, 1.0f);
	shaking_remaining_time_ = 0.0f;
	changed_ = true;
}