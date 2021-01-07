#include "Controller.h"

void Controller::Update()
{
	Window* win = Input::GetFocusedWindow();
	if (win == nullptr)
		return;
	if (Input::GetKey(Key::MouseMiddle).value == KeyPressed)
	{
		win->LockCursor(win->GetSize() / 2);
		win->ShowCursor(false);
	}
	if (Input::GetKey(Key::MouseMiddle).value == KeyReleased)
	{
		win->UnlockCursor();
		win->ShowCursor(true);
	}
	if (Input::GetKey(Key::MouseMiddle).value == KeyDown)
	{
		Transform3D* t = GetEntity().GetComponent<Transform3D>();
		Vec2f m = Input::GetMouseMovement();
		rot.x += m.y * 0.001;
		rot.y += m.x * 0.001;
		t->rot = Quatf(AxisAnglef(Vec3f(0, 1, 0), rot.y)) * Quatf(AxisAnglef(Vec3f(1, 0, 0), rot.x));
	}
}