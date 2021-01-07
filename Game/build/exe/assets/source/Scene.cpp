#include "Scene.h"

#include "Controller.h"

MainScene::MainScene()
{
}

MainScene::~MainScene()
{ 
}

void MainScene::Load()
{
	{
		Shader vertex = Shader(VertexShader, "assets/shaders/terrainVertex.glsl");
		Shader fragment = Shader(FragmentShader, "assets/shaders/terrainFragment.glsl");

		terrainMaterial.SetShaders({ &vertex, &fragment });
	}

	terrain.Setup();	
	terrain.SetMaterial(&terrainMaterial, { Type::Float });

	cameraEntity = CreateEntity<Transform3D, Camera, Controller>("Main Camera");
	{
		Camera* camera = cameraEntity.GetComponent<Camera>();
		camera->projectionMatrix = Math::PerspectiveMatrix<float>(90, Renderer::GetViewportRatio(), 0.1, 1000);
	}
}

void MainScene::Save()
{
}

void MainScene::Unload()
{
	terrain.Cleanup();
}

void MainScene::Start()
{
}

void MainScene::Update(float dt)
{
	UpdateSystem<TransformSystem>();

	Renderer::ClearTarget();

	terrain.Render(cameraEntity);

	Renderer::UpdateTarget();
}

void MainScene::Stop()
{
}
