#pragma once
#define BLAZE_ONLY_CORE
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

#include "Transform.h"
#include "Object.h"
#include "Sprite.h"

enum CameraProjection
{
	Perspective,
	Orthographic,
};

class Camera : public Object
{
public:
	mat4f projMat, viewMat;
	union
	{
		struct {
			float fov, aspectRatio;
		} Perspective;
		struct {
			float left, right, bottom, top;
		} Orthographic;
	} Projection;
	float far;
	float near;
	CameraProjection mode;

	Camera(float fov, float aspectRatio, float near, float far)
	{
		SetPerspective(fov, aspectRatio, near, far);
	}
	Camera(float left, float right, float bottom, float top, float near, float far)
	{
		SetOrthographic(left, right, bottom, top, near, far);
	}
	Camera(float zoom, float aspectRatio, float near, float far, bool lockX)
	{
		SetOrthographic(zoom, aspectRatio, near, far, lockX);
	}

	void SetPerspective(float fov, float aspectRatio, float near, float far)
	{
		Projection.Perspective.fov = fov;
		Projection.Perspective.aspectRatio = aspectRatio;
		this->near = near;
		this->far = far;
		mode = Perspective;
		UpdateProjectionMatrix();
	}
	void SetOrthographic(float left, float right, float bottom, float top, float near, float far)
	{
		Projection.Orthographic.left = left;
		Projection.Orthographic.right = right;
		Projection.Orthographic.bottom = bottom;
		Projection.Orthographic.top = top;
		mode = Orthographic;
		this->near = near;
		this->far = far;
		UpdateProjectionMatrix();
	}
	void SetOrthographic(float zoom, float aspectRatio, float near, float far, bool lockX)
	{
		Projection.Orthographic.left = -zoom * (lockX ? 1 : aspectRatio);
		Projection.Orthographic.right = zoom * (lockX ? 1 : aspectRatio);
		Projection.Orthographic.bottom = -zoom / (lockX ? aspectRatio : 1);
		Projection.Orthographic.top = zoom / (lockX ? aspectRatio : 1);
		this->near = near;
		this->far = far;
		mode = Orthographic;
		UpdateProjectionMatrix();
	}

	void UpdateProjectionMatrix()
	{
		if (mode == Perspective)
			projMat = Math::PerspectiveMatrix(Projection.Perspective.fov, near, far, Projection.Perspective.aspectRatio);
		else if (mode == Orthographic)
			projMat = Math::OrtographicMatrix(Projection.Orthographic.left, Projection.Orthographic.right, Projection.Orthographic.bottom, Projection.Orthographic.top, near, far);
	}
	void UpdateViewMatrix()
	{
		viewMat = Math::RotationMatrix(Math::Inverse(tran.rot)) * Math::ScalingMatrix(tran.scale) * Math::TranslationMatrix(-tran.pos);
	}

	void Update() override
	{
		UpdateProjectionMatrix();
		UpdateViewMatrix();
		Object::Update();
	}	
};