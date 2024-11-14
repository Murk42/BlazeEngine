#include "pch.h"
#include "CameraController.h"

CameraController::CameraController() :
	window(nullptr), speed(1), dt(0.0f)
{
	Input::GetInputEventSystem().inputPostUpdateDispatcher.AddHandler(*this);
}

CameraController::~CameraController()
{
	if (this->window != nullptr)
	{		
		this->window->mouseScrollDispatcher.RemoveHandler(*this);
		this->window->mouseMotionDispatcher.RemoveHandler(*this);
	}
}

void CameraController::SetWindow(WindowBase* window)
{
	if (this->window != nullptr)
	{				
		this->window->mouseScrollDispatcher.RemoveHandler(*this);
		this->window->mouseMotionDispatcher.RemoveHandler(*this);
	}

	this->window = window;

	if (this->window != nullptr)
	{
		this->window->mouseScrollDispatcher.AddHandler(*this);
		this->window->mouseMotionDispatcher.AddHandler(*this);
	}
}

Mat4f CameraController::GetViewMatrix() const
{
	return
		Mat4f::RotationMatrix(rotation.Conjugated()) *
		Mat4f::TranslationMatrix(-position);
}

void CameraController::OnEvent(Input::Events::MouseScroll event)
{
	speed *= Math::Pow(1.1, event.value);
}

void CameraController::OnEvent(Input::Events::MouseMotion event)
{  
	if (window->GetLastKeyState(Key::MouseRight).down)
	{
		angles.x += -event.delta.y * 0.001;
		angles.y += event.delta.x * 0.001;

		rotation = Quatf(Vec3f(0, 1, 0), angles.y) * Quatf(Vec3f(1, 0, 0), angles.x);
	}
}

void CameraController::OnEvent(Input::Events::InputPostUpdateEvent event)
{
	dt = dtStopwatch.Reset();

	if (window == nullptr)
		return;

	if (window->GetLastKeyState(Key::W).down)
		position += rotation * Vec3f(0, 0, speed * dt);
		
	if (window->GetLastKeyState(Key::A).down)
		position += rotation * Vec3f(-speed * dt, 0, 0);		

	if (window->GetLastKeyState(Key::S).down)
		position += rotation * Vec3f(0, 0, -speed * dt);
		
	if (window->GetLastKeyState(Key::D).down)
		position += rotation * Vec3f(speed * dt, 0, 0);
		
	if (window->GetLastKeyState(Key::Space).down)
		position += Vec3f(0, speed * dt, 0);
		
	if (window->GetLastKeyState(Key::RShift).down || window->GetLastKeyState(Key::LShift).down)
		position += Vec3f(0, -speed * dt, 0);

}
