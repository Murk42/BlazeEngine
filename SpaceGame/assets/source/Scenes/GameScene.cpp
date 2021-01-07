#include "GameScene.h"

void GameScene::Load()
{
	/*Load skybox*/ {
		skyboxCubemapSpace.Load("assets/textures/spaceCubemap/spaceCubemap.png", CubemapFileType::Column);

		Shader fragmentShader(FragmentShader, "assets/shaders/skyboxFragment.glsl");
		Shader vertexShader(VertexShader, "assets/shaders/skyboxVertex.glsl");
		skyboxSP.SetShaders({ &fragmentShader, &vertexShader });
		skyboxUniformProjection = skyboxSP.GetUniform("u_projection");
		skyboxUniformView = skyboxSP.GetUniform("u_view");

		float vertex[] {
			-1, -1, -1,
			1, -1, -1,
			1, 1, -1,
			-1, 1, -1,
			-1, -1, 1,
			1, -1, 1,
			1, 1, 1,
			-1, 1, 1
		};
		unsigned int index[] {
			0, 1, 3, 3, 1, 2,
			1, 5, 2, 2, 5, 6,
			5, 4, 6, 6, 4, 7,
			4, 0, 7, 7, 0, 3,
			3, 2, 7, 7, 2, 6,
			4, 5, 0, 0, 5, 1
		};

		skyboxVB.AllocateData(vertex, 8 * 3 * 4, Static | Draw);
		skyboxVL.SetLayout({ Type::Vec3f }, &skyboxVB);		
		skyboxIB.AllocateData(index, 6 * 2 * 3, Static | Draw, Type::UnsignedInt);
	}

	Entity cameraEntity = CreateEntity<Transform, Camera, Controller>("Camera", Transform(), Camera(), Controller());
	camera = cameraEntity.GetComponent<Camera>();
	camera->projectionMatrix = Math::PerspectiveMatrix<float>(Math::Radians<float>(90), 1, 0.01, 100);
	cameraTransform = cameraEntity.GetComponent<Transform>();
}
void GameScene::Update(float dt)
{
	UpdateSystem<Controller::BehaviourSystem>();

	Renderer::ClearTarget();
	Renderer::UseDepthBuffer(false);	

	/*Render skybox*/ {
		skyboxVB.Bind();
		skyboxVL.Bind(); 
		skyboxIB.Bind();

		skyboxSP.SetUniform(skyboxUniformProjection, &camera->projectionMatrix);
		skyboxSP.SetUniformValue(skyboxUniformView, Math::ViewMatrix(cameraTransform->pos, cameraTransform->scale, cameraTransform->rot));
		skyboxSP.SetUniformValue("u_texture", 0);
		skyboxSP.Bind();

		skyboxCubemapSpace.Bind(0);

		Renderer::DrawTriangles();
	}

	Renderer::UseDepthBuffer(true);


	UpdateSystem<RenderSystem>();


	Renderer::UpdateTarget();
}