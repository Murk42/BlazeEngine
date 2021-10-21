#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;
using namespace Blaze::Graphics;
using namespace Blaze::Graphics::Core;

#include "Planet/Planet.h"
#include "Star/Star.h"

class App : public BlazeEngine<App>
{
public:
	static constexpr float farNearProjLimit = 1024.0f;
	static constexpr float nearPlaneNearProj = 1.0f;
	static constexpr float farPlaneNearProj = farNearProjLimit + 128.0f;
	static constexpr float nearPlaneFarProj = 1.0f;
	static constexpr float farPlaneFarProj = 1024.0f * 1024.0f;

	Window window;

	Font font;
	Charmap charmap;		

	Program planetProgram;
	PlanetMaterial planetMaterial;
	std::vector<Planet> planets;	

	Program starProgram;
	StarMaterial starMaterial;
	Star star;
	

	struct {
		float moveSpeed = 100.0f;

		Vec3f globalPosition;

		Vec2f angles;
		Vec3f positionDelta;

		Transform3D transform;
	} camera;

	Mat4f farProj3D;
	Mat4f nearProj3D;
	Mat4f proj2D;

	Mat4f farVP3D;
	Mat4f nearVP3D;
	Mat4f VP2D;

	float dt;
	Stopwatch dtStopwatch;

	void SetupEvents();
	void SetupWindow();
	void SetupRenderer();
	void Startup() override;

	void UpdateEngine();
	void UpdateTransforms();
	void UpdateInput();
	void UpdateUI();
	void Render(); 
	void Frame() override;

	void Set3DProjectionMatrix();
	void Set2DProjectionMatrix(Vec2i windowSize);	

	InputEventFunction<InputEventType::WindowClosed> WindowClosed = [&](Window&) {
		Stop();
	};
	InputEventFunction<InputEventType::WindowResized> WindowResized = [&](int w, int h, Window&) {
		Renderer::SetViewport({ 0, 0 }, { w, h });
		Set3DProjectionMatrix();
		Set2DProjectionMatrix(Vec2i(w, h));
	};
};

extern App* app;