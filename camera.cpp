#include "camera.h"
#include "graphics/sprite_renderer.h"
#include "maths/math_utils.h"

Camera::Camera(abfw::SpriteRenderer* renderer) :
	renderer_(renderer),
	changed_(false)
{
	rotation_ = 0.0f; // rotation will always be 0 to begin with
	projection_matrix_ = renderer_->projection_matrix(); // initialize the projectile matrix to the vita matrix
}


Camera::~Camera(void)
{
}

void Camera::MoveBy(abfw::Vector2 translation)
{
	abfw::Vector3 current_pos = renderer_->projection_matrix().GetTranslation();
	abfw::Vector3 new_pos = abfw::Vector3(current_pos.x + translation.x, current_pos.y + translation.y, current_pos.z);
	projection_matrix_.SetTranslation(new_pos);
	changed_ = true;
}

void Camera::MoveTo(abfw::Vector2 translation)
{
	float z = renderer_->projection_matrix().GetTranslation().z; // still need to get z
	projection_matrix_.SetTranslation(abfw::Vector3(translation.x, translation.y, z));
	changed_ = true;
}

void Camera::Scale(abfw::Vector3 scalefactor)
{
	projection_matrix_.Scale(scalefactor);
	changed_ = true;
}

void Camera::Rotate(float deg)
{
	rotation_ += deg;
	changed_ = true;
}

void Camera::updateCamera()
{
	if(changed_) // only update the matrix on frames that it has changed
	{
		projection_matrix_.RotationZ(abfw::DegToRad(rotation_));
		renderer_->set_projection_matrix(projection_matrix_);
	}
}