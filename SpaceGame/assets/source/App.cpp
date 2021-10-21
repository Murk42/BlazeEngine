#include "App.h"

App* app;

void App::SetupEvents()
{
	Input::SetEventFunction<InputEventType::WindowClosed>(WindowClosed);
	Input::SetEventFunction<InputEventType::WindowResized>(WindowResized);
} 
void App::SetupWindow()
{
	window.SetWindowed(true, false); 
	window.ShowWindow(true);
}
void App::SetupRenderer()
{
	Renderer::SetTarget(window);
	Renderer::SetViewport(Vec2i(), window.GetSize());
}
void App::Startup() 
{
	app = this;
	SetupEvents();
	SetupWindow();
	SetupRenderer();

	Set3DProjectionMatrix();
	Set2DProjectionMatrix(window.GetSize());

	font.Load("assets/default/fonts/Consola.ttf");
	font.LoadCharmap(charmap, 20);	

	planetProgram.LoadShaders(
		"assets/world/planet/shaders/vertex.vert",
		nullptr,
		"assets/world/planet/shaders/fragment.frag"
	);
	planetMaterial.SetProgram(&planetProgram);

	starProgram.LoadShaders(
		"assets/world/star/shaders/vertex.vert",
		nullptr,
		"assets/world/star/shaders/fragment.frag"
	);
	starMaterial.SetProgram(&starProgram);	

	{
		star.Initialize(Vec3f(-10000.0f, 0, 0), 1000.0f);
	}

	{
		Planet& planet = planets.emplace_back();
		planet.Initialize(1000.0f, Vec3f(0, 0, 10000.0f), &star);
	}
}	
 
void App::UpdateEngine()
{	
	Input::Update();
}
void App::UpdateTransforms()
{
	star.UpdateTransform(camera.positionDelta);

	for (auto& planet : planets)
		planet.UpdateTransform(camera.positionDelta);	

	camera.transform.UpdateAsViewTransform();			

	nearVP3D = nearProj3D * camera.transform.mat;
	farVP3D = farProj3D * camera.transform.mat;
	VP2D = proj2D;
}
void App::UpdateInput()
{		
	camera.positionDelta = Vec3f();

	static constexpr float cameraRotateSpeed = 0.01f;

	int scroll = Input::GetMouseScroll();
	while (scroll > 0)
	{
		camera.moveSpeed *= 1.02;
		--scroll;
	}
	while (scroll << 0)
	{
		camera.moveSpeed /= 1.02;
		++scroll;
	}
	  
	if (Input::GetKeyState(Key::W).state == KeyState::Down)
		camera.positionDelta += camera.transform.rot * Vec3f(0, 0, 1) * camera.moveSpeed * dt;		
	if (Input::GetKeyState(Key::D).state == KeyState::Down)
		camera.positionDelta += camera.transform.rot * Vec3f(1, 0, 0) * camera.moveSpeed * dt;
	if (Input::GetKeyState(Key::S).state == KeyState::Down)
		camera.positionDelta += camera.transform.rot * Vec3f(0, 0,-1) * camera.moveSpeed * dt;
	if (Input::GetKeyState(Key::A).state == KeyState::Down)
		camera.positionDelta += camera.transform.rot * Vec3f(-1, 0, 0) * camera.moveSpeed * dt;
	if (Input::GetKeyState(Key::Space).state == KeyState::Down)
		camera.positionDelta += Vec3f(0, 1, 0) * camera.moveSpeed * dt;
	if (Input::GetKeyState(Key::LShift).state == KeyState::Down)
		camera.positionDelta += Vec3f(0, -1, 0) * camera.moveSpeed * dt;
	  
	if (Input::GetKeyState(Key::MouseRight).state == KeyState::Down) 
		Input::LockCursor(true);
	if (Input::GetKeyState(Key::MouseRight).state == KeyState::Up)
		Input::LockCursor(false);

	if (Input::GetKeyState(Key::MouseRight).state == KeyState::Down)
	{			 
		Vec2i mm = Input::GetMouseMovement(); 
		camera.angles.x += mm.y * cameraRotateSpeed;
		camera.angles.y += mm.x * cameraRotateSpeed;

		camera.transform.rot = Quatf(AxisAnglef(Vec3f(0, 1, 0), camera.angles.y)) * Quatf(AxisAnglef(Vec3f(1, 0, 0), camera.angles.x));
	}

	camera.globalPosition += camera.positionDelta;
} 
void App::UpdateUI()
{	
}
void App::Render()
{
	Renderer::ClearTarget();

	if (star.transform.pos.SqrLenght() >= farNearProjLimit * farNearProjLimit)
		star.Render(farVP3D);

	for (auto& planet : planets)
		if (planet.transform.pos.SqrLenght() >= farNearProjLimit * farNearProjLimit)
			planet.Render(farVP3D);

	Renderer::ClearTargetDepth();

	if (star.transform.pos.SqrLenght() < farNearProjLimit * farNearProjLimit)
		star.Render(nearVP3D);

	for (auto& planet : planets)
		if (planet.transform.pos.SqrLenght() < farNearProjLimit * farNearProjLimit)
			planet.Render(nearVP3D);

	//UI
	Renderer::ClearTargetDepth();
	
	Debug::Write("Camera speed: " + String::Convert(camera.moveSpeed)						, ColorRGBA(255, 255, 255, 255), Vec2f(10, -10), Align::TopLeft, charmap);
	Debug::Write("Planet distance: " + String::Convert(planets[0].transform.pos.Lenght())	, ColorRGBA(255, 255, 255, 255), Vec2f(10, -30), Align::TopLeft, charmap);
	Debug::Write("Planet pos: " + String::Convert(planets[0].transform.pos)					, ColorRGBA(255, 255, 255, 255), Vec2f(10, -50), Align::TopLeft, charmap);

	Renderer::UpdateTarget();
}
void App::Frame()
{
	dt = dtStopwatch.Reset();

	UpdateEngine();
	UpdateInput();

	for (auto& planet : planets)
		planet.UpdateChunks(camera.transform);

	UpdateUI();
	UpdateTransforms();		
	Render();	
}

void App::Set3DProjectionMatrix()
{
	nearProj3D = Math::PerspectiveMatrix<float>(90, Renderer::GetViewportRatio(), nearPlaneNearProj, farPlaneNearProj);
	farProj3D = Math::PerspectiveMatrix<float>(90, Renderer::GetViewportRatio(), nearPlaneFarProj, farPlaneFarProj);
}
void App::Set2DProjectionMatrix(Vec2i windowSize)
{
	proj2D = Math::OrthographicMatrix<float>(0, windowSize.x, 0, windowSize.y, -1, 1);
}	
