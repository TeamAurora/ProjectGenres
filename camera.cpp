#include "camera.h"
#include "graphics/sprite_renderer.h"
#include "maths/math_utils.h"
#include "system/platform.h"

Camera::Camera(abfw::SpriteRenderer* renderer, abfw::Platform& platform) :
	renderer_(renderer),
	platform_(platform)
{
	translation_ = abfw::Vector2(0.0f, 0.0f);
	rotation_ = 0.0f;
	scale_ = abfw::Vector2(1.0f, 1.0f);
	changed_ = false;
}


Camera::~Camera(void)
{
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

void Camera::ApplyCameraTransforms()
{
	if(changed_) // only update the matrix on frames that it has changed
	{
		abfw::Matrix44 result;

		// construct edges of the frustrum using transforms
		float left_edge = translation_.x * scale_.x;
		float right_edge = (translation_.x + platform_.width()) * scale_.x;
		float top_edge = translation_.y * scale_.y;
		float bottom_edge = (translation_.y + platform_.height()) * scale_.y;
		
		// construct the frustrum from parameters
		result.OrthographicFrustumGL(left_edge, right_edge, top_edge, bottom_edge, -1.0f, 1.0f);

		/*abfw::Matrix44 rotation;
		rotation.RotationZ(rotation_);
		result = result * rotation;*/

		// push transformed frustrum to gfx renderer
		renderer_->set_projection_matrix(result);
	}
}

void Camera::ResetCamera()
{
	translation_ = abfw::Vector2(0.0f, 0.0f);
	rotation_ = 0.0f;
	scale_ = abfw::Vector2(1.0f, 1.0f);
	changed_ = true;
}