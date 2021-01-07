
#include "BlazeEngine/BlazeEngineMain.h"
using namespace Blaze;

#include "Renderable.h"
#include "Sprite.h"
#include "Model.h"
#include "Skybox.h"

#include "Utilities.h"

#include "Object.h"
#include "Transform.h"
#include "Camera.h"
#include "SpaceShip.h"

static void SetupEvents();
class App : public Application::Base<App>
{
public:
	Console::Stack errorStack = Console::Stack({ 0, 0 }, { 100, 20 }, true);
	Console::Frame profilerFrame = Console::Frame({ 0, 20 }, { 50, 20 }, true);
	Console::VariableFrame variableFrame = Console::VariableFrame({ 50, 20 }, { 50, 20 }, true);
	Window win;	

	Shader vertexShader			= Shader(VertexShader, "assets/shaders/vertex.glsl");
	Shader fragmentShader		= Shader(FragmentShader, "assets/shaders/fragment.glsl");
	Shader skyboxVertexShader	= Shader(VertexShader, "assets/shaders/skyboxVertex.glsl");
	Shader skyboxFragmentShader = Shader(FragmentShader, "assets/shaders/skyboxFragment.glsl");
	Shader spriteVertexShader	= Shader(VertexShader, "assets/shaders/spriteVertex.glsl");
	Shader spriteFragmentShader	= Shader(FragmentShader, "assets/shaders/spriteFragment.glsl");

	Camera camera = Camera(90, 1, 0.1, 10000);
	Camera uiCamera = Camera(1, 1, -1, 1, false);

	Material planetMaterial = Material({ &fragmentShader, &vertexShader });
	Mesh planetMesh = Mesh("assets/meshes/planet.obj");
	Texture2D planetTexture = Texture2D("assets/default/default.png");
	Model planetModel = Model({ &planetMaterial }, &planetMesh);
	Object planet = Object(&planetModel, Transform({ 0, 0, 5000 }, { 100, 100, 100 }, {}));

	Material shipMaterial = Material({ &fragmentShader, &vertexShader });
	Mesh shipMesh = Mesh("assets/meshes/untitled.obj");
	Texture2D shipTexture = Texture2D("assets/default/default.png");
	Model shipModel = Model({ &shipMaterial }, &shipMesh);
	SpaceShipObject ship = SpaceShipObject(100);
	SpaceShipController shipController = SpaceShipController(&camera, &ship, true);

	Material skyboxMaterial = Material({ &skyboxFragmentShader, &skyboxVertexShader });
	TextureCubemap cubemap = TextureCubemap("assets/textures/px.png", "assets/textures/nx.png", "assets/textures/py.png", "assets/textures/ny.png", "assets/textures/pz.png", "assets/textures/nz.png");
	Skybox skybox = Skybox(&cubemap, &skyboxMaterial); 		

	Material uiMaterial = Material({ &spriteFragmentShader, &spriteVertexShader });
	Texture2D cursorTexture = Texture2D("assets/textures/cursor.png");
	SpriteBatch uiBatch = SpriteBatch(&uiMaterial, &cursorTexture);

	Sprite cursorSprite = Sprite(&uiBatch, Transform(-0.05, 0.1, quatf()), 0, 64);	

	void Startup() override
	{		  				
		Skybox::Setup();		

		camera.SetPerspective(90, 1, 0.1, 10000);
		uiCamera.SetOrthographic(1,	1, -1, 1, false);		

		SetupEvents();
		SetupResources();
		   
		win.SetWindowed(true, false); 
		win.ShowWindow(true);
		Renderer::SetTarget(win);  
		Profiler::SetPerformanceFrame(&profilerFrame);		
	}
	 
	void Cleanup() override
	{
		Skybox::Cleanup();
	}	

	void Frame() override
	{		  		
		Profiler::Update();
		Input::Update();
		   
		if (Input::GetKey(Key::MouseMiddle).value == KeyPressed)
		{
			win.LockCursor(win.GetSize() / 2);
			win.ShowCursor(false);
		}
		if (Input::GetKey(Key::MouseMiddle).value == KeyReleased)
		{			
			win.UnlockCursor();
			win.ShowCursor(true);			
		}				
	 
		cursorSprite.tran.pos 
			= As<Vec2f>(shipController.shipRotations) / 10  * Vec2f(1, -1.0f) - 0.05f;

		ship.Update();		
		shipController.Update();
		
		cursorSprite.Update();
		planet.Update();
		uiBatch.Update();

		camera.Update();
		uiCamera.Update();

		Renderer::ClearTargetDepth();		

		Renderer::UseDepthBuffer(false);
				
		skybox.Render(&camera);
		
		Renderer::UseDepthBuffer(true);

		planetModel.Render(&camera, &planet.tran);
		shipModel.Render(&camera, &ship.tran);		
		uiBatch.Render(&uiCamera);
		Renderer::UpdateTarget();
	}

	void Error(ErrorData error) override
	{
		errorStack.Add(ErrorTypeToString(error.type) + "(" + ErrorImportanceToString(error.importance) + ") from " + error.source + ": " + error.message);
		errorStack.Clear();
		errorStack.Refresh();
 	}

	Vec2f GetScreenPos(Vec2f pixel)
	{
		return (pixel / win.GetSize() * 2.0f - 1.0f) * Vec2f(camera.Projection.Perspective.aspectRatio, -1.0f);
	}

	void SetupResources()
	{		
		shipTexture.GenerateMipmaps();
		shipTexture.SetSettings(Linear, Linear, Linear);		
		cubemap.GenerateMipmaps();
		cubemap.SetSettings(Linear, Linear, Linear);				
				
		shipModel.materials[0]->GetProperty("u_texture")->SetData(shipTexture);
		planetModel.materials[0]->GetProperty("u_texture")->SetData(planetTexture);
		
		uiBatch.AddSprite(&cursorSprite);
		uiBatch.UpdateMesh();

		shipController.SetupVariableFrame(&variableFrame);		
	}
};

#include "Events.h"

static void SetupEvents()
{
	WindowResizeFunc(App::Instance()->win.GetSize().x, App::Instance()->win.GetSize().y, &App::Instance()->win);
	Input::AddEventFunction(WindowResized, WindowResizeFunc);
	Input::AddEventFunction(WindowClose, WindowCloseFunc);
}