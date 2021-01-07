#pragma once
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

class Controller : public Behaviour<Controller>
{
public:
	Vec2f rot;  
	float fov;
	Controller()
		: rot(0, 0), fov(90)
	{
	}
	Controller(const Controller& c)
		: rot(c.rot), fov(c.fov)
	{

	}

	void Update()
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
			Transform* t = GetEntity().GetComponent<Transform>();
			Vec2f m = Input::GetMouseMovement();
			rot.x += m.y * 0.001;
			rot.y += m.x * 0.001;
			t->rot = Quatf(AxisAnglef(Vec3f(0, 1, 0), rot.y)) * Quatf(AxisAnglef(Vec3f(1, 0, 0), rot.x));
		}				
	}
};

class GameScene : public Scene<GameScene>
{
public:		
	TextureCubemap skyboxCubemapSpace;
	Uniform* skyboxUniformProjection;
	Uniform* skyboxUniformView;
	ShaderProgram skyboxSP;
	VertexLayout skyboxVL;
	VertexBuffer skyboxVB;
	IndexBuffer skyboxIB;	

	Camera* camera;
	Transform* cameraTransform;
	
	void Load();
	void Update(float dt);
};