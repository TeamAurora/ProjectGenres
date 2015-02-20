#pragma once
#include "maths/matrix44.h"
#include "maths/vector2.h"
#include "maths/vector3.h"

namespace abfw
{
	class SpriteRenderer;
	class Platform;
}


class Camera
{
public:
	Camera(abfw::SpriteRenderer* renderer, abfw::Platform& platform);
	~Camera(void);

	// Move function that assumes the movement is relative to current position (relative transform)
	void MoveBy(abfw::Vector2 translation);
	// Move function that assumes the movement disregards current position (absolute transform)
	void MoveTo(abfw::Vector2 position);

	// Scale function that sets scale relative to original scale (absolute transform)
	void Scale(abfw::Vector2 scalefactor);

	// Rotate function that adds to the current rotation (relative transform)
	// Rotation currently does not work correctly
	//void Rotate(float deg);

	void ApplyCameraTransforms();
	void ResetCamera();

private:

	bool changed_; // Tracks if camera has been changed to stop unnecessary updates
	abfw::SpriteRenderer* renderer_; // pointer to sprite renderer for updating its projection matrix
	abfw::Vector2 translation_;
	abfw::Vector2 scale_;
	float rotation_;
	const abfw::Platform& platform_;
};