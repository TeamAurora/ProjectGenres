#pragma once
#include "maths/matrix44.h"
#include "maths/vector2.h"
#include "maths/vector3.h"

namespace abfw
{
	class SpriteRenderer;
}


class Camera
{
public:
	Camera(abfw::SpriteRenderer* renderer);
	~Camera(void);

	// Move function that assumes the movement is relative to current position (relative transform)
	void MoveBy(abfw::Vector2 translation);
	// Move function that assumes the movement disregards current position (absolute transform)
	void MoveTo(abfw::Vector2 translation);

	// Scale function that sets scale relative to original scale (absolute transform)
	void Scale(abfw::Vector3 scalefactor);

	// Rotate function that adds to the current rotation (relative transform)
	void Rotate(float deg);

	void updateCamera();

private:

	bool changed_; // Tracks if camera has been changed to stop unnecessary updates
	float rotation_; // rotation amount in degrees
	abfw::SpriteRenderer* renderer_; // pointer to sprite renderer for updating its projection matrix
	abfw::Matrix44 projection_matrix_; // projection matrix stored with this camera
};