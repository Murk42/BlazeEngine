#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;
using namespace Blaze::Graphics::Core;

class App : public BlazeEngine<App>
{
public:	
	Window window; 

	Font font;
	Charmap charmap;

	Program program;	
	int mvpUniform;
	int tessellationUniform;
	int diffuseUniform;
	int normalUniform;
	int displacementUniform;
	int displacementAmountUniform;
	int rotationUniform;
	int lightDirUniform;	

	Texture2D brickDisplacement;
	Texture2D brickDiffuse;
	Texture2D brickNormal;

	VertexArray va;
	GraphicsBuffer vb;	
	size_t vertexCount;

	Mat4f proj;
	Mat4f view;
	Transform3D transform;

	void InitializeMesh()
	{
		struct Vertex
		{
			Vec3f pos;
			Vec2f uv;
			Vec3f normal;			
		};		
		constexpr size_t lod = 64;
		constexpr float zStep = 1.0f / lod;
		constexpr float xStep = 1.0f / lod;
		vertexCount = lod * lod * 4;
		Vertex vertices[lod * lod * 4];		

		unsigned index = 0;
		float offsetZ = 0.0f;
		float offsetX = 0.0f;
		for (size_t z = 0; z < lod; ++z)
		{
			for (size_t x = 0; x < lod; ++x)
			{
				vertices[index + 0].pos = Vec3f(offsetX - 0.5f        , 0.0f, offsetZ - 0.5f       );
				vertices[index + 1].pos = Vec3f(offsetX - 0.5f + xStep, 0.0f, offsetZ - 0.5f       );
				vertices[index + 2].pos = Vec3f(offsetX - 0.5f        , 0.0f, offsetZ - 0.5f+ zStep);
				vertices[index + 3].pos = Vec3f(offsetX - 0.5f + xStep, 0.0f, offsetZ - 0.5f+ zStep);

				vertices[index + 0].uv = Vec2f(offsetX        , offsetZ        );
				vertices[index + 1].uv = Vec2f(offsetX + xStep, offsetZ        );
				vertices[index + 2].uv = Vec2f(offsetX        , offsetZ + zStep);
				vertices[index + 3].uv = Vec2f(offsetX + xStep, offsetZ + zStep);

				vertices[index + 0].normal = Vec3f(0.0f, 1.0f, 0.0f);
				vertices[index + 1].normal = Vec3f(0.0f, 1.0f, 0.0f);
				vertices[index + 2].normal = Vec3f(0.0f, 1.0f, 0.0f);
				vertices[index + 3].normal = Vec3f(0.0f, 1.0f, 0.0f);

				offsetX += xStep;
				index += 4;
			}

			offsetZ += zStep;
			offsetX = 0.0f;
		}

		vb.AllocateStaticStorage(BufferView(vertices, sizeof(vertices)));				
		 
		va.SetVertexAttributeData(0, VertexAttribute(&vb, sizeof(Vertex), 0, VertexAttributeType::Float, 3, false, 0));		
		va.SetVertexAttributeData(1, VertexAttribute(&vb, sizeof(Vertex), 0, VertexAttributeType::Float, 2, false, sizeof(float) * 3));
		va.SetVertexAttributeData(2, VertexAttribute(&vb, sizeof(Vertex), 0, VertexAttributeType::Float, 3, false, sizeof(float) * 5));		
	}

	void Startup() override
	{				
		Input::SetEventFunction<InputEventType::WindowClosed>(WindowClosed);		
		Input::SetEventFunction<InputEventType::WindowResized>(WindowResized);

		window.SetWindowed(true, false);		
		window.ShowWindow(true);

		Renderer::SetTarget(window);		
		Renderer::DoFaceCulling(false);
		Renderer::DoBlending(true);
		Renderer::SetViewport(Vec2i(), window.GetSize());
		proj = Math::PerspectiveMatrix<float>(90, Renderer::GetViewportRatio(), 0.1, 10);

		font.Load("assets/default/fonts/Consola.ttf");
		font.LoadCharmap(charmap, 20);
		  
		VertexShader vertShader = { "assets/shader.vert" };
		TessellationControlShader tescShader = { "assets/shader.tesc" };
		TessellationEvaluationShader teseShader = { "assets/shader.tese" };
		GeometryShader geomShader = { "assets/shader.geom" };
		FragmentShader fragShader = { "assets/shader.frag" };
		program.LinkShaders({ &vertShader, &tescShader, &teseShader, &geomShader, &fragShader });

		mvpUniform = program.GetUniformLocation("u_MVP");
		tessellationUniform = program.GetUniformLocation("u_tessellation");
		diffuseUniform = program.GetUniformLocation("u_diffuse");
		normalUniform = program.GetUniformLocation("u_normal");
		displacementUniform = program.GetUniformLocation("u_displacement");
		displacementAmountUniform = program.GetUniformLocation("u_displacementAmount");
		rotationUniform = program.GetUniformLocation("u_rotation");
		lightDirUniform = program.GetUniformLocation("u_lightDir");

		InitializeMesh();
		 
		brickDisplacement.Load("assets/brickDisplacement.jpg");
		brickDisplacement.GenerateMipmaps();
		//{
		//	Bitmap bm;
		//	ColorRGB color = { 0, 255, 0 };
		//	bm.Create(Vec2i(1, 1), PixelFormat::RGB, &color);
		//	brickNormal.Create(bm);
		//}
		brickNormal.Load("assets/brickNormal.jpg");
		brickNormal.GenerateMipmaps();
		brickDiffuse.Load("assets/brickDiffuse.jpg");
		brickDiffuse.GenerateMipmaps();

		view = Mat4f::Identity();
		transform.pos = Vec3f(0, 0, 1);
		transform.rot = Quatf(AxisAnglef(Vec3f(1, 0, 0), Math::Radians<float>(-90)));
	}

	float tessellationAmount = 1.0f;
	float dt;
	uint FPS, FPSCount = 0;
	Stopwatch FPSTimer;
	Stopwatch dtTimer;

	void Frame() override
	{			
		if (FPSTimer.GetTime() >= 1.0f)
		{
			FPS = FPSCount;
			FPSCount = 0;
			FPSTimer.Reset();
		}
		++FPSCount;

		dt = dtTimer.Reset();
		Input::Update();	

		if (Input::GetKeyState(Key::MouseRight).state == KeyState::Down)
		{
			transform.rot *= Quatf(AxisAnglef(Vec3f(1, 0, 0), -Input::GetMouseMovement().y * dt * 3));
			transform.rot *= Quatf(AxisAnglef(Vec3f(0, 1, 0), -Input::GetMouseMovement().x * dt * 3));
		}

		if (Input::GetKeyState(Key::Q).state == KeyState::Down)
			transform.rot *= Quatf(AxisAnglef(Vec3f(0, 0, 1), dt));
		if (Input::GetKeyState(Key::E).state == KeyState::Down)
			transform.rot *= Quatf(AxisAnglef(Vec3f(0, 0, 1), -dt));

		if (Input::GetKeyState(Key::T).state == KeyState::Pressed)
			ConsoleMode();

		transform.Update();

		tessellationAmount += Input::GetMouseScroll() * 0.5f;
		if (tessellationAmount < 1.0f)
			tessellationAmount = 1.0f;
		if (tessellationAmount > 64.0f)
			tessellationAmount = 64.0f;

		Renderer::ClearTarget();

		program.Use();
		program.SetUniform(mvpUniform, proj * view * transform.mat);
		program.SetUniform(tessellationUniform, tessellationAmount);
		program.SetUniform(diffuseUniform, 0);
		program.SetUniform(normalUniform, 1);
		program.SetUniform(displacementUniform, 2);
		program.SetUniform(displacementAmountUniform, 0.02f);
		program.SetUniform(rotationUniform, Math::RotationMatrix<float>(transform.rot));
		program.SetUniform(lightDirUniform, Vec3f(1, 0, 0).Normalised());

		brickDiffuse.Bind(0);
		brickNormal.Bind(1);
		brickDisplacement.Bind(2);

		VertexArray::Bind(va);
		
		if (Input::GetKeyState(Key::W).state == KeyState::Down)
			Renderer::RenderWireframe(true);
		Renderer::SetPatchSize(4);

		//Renderer::RenderPatches(IndexType::UnsignedInt, vertexCount);	
		Renderer::RenderPatchArray(vertexCount, 0);

		if (Input::GetKeyState(Key::W).state == KeyState::Down)
			Renderer::RenderWireframe(false);

		Debug::RenderTexture(brickDiffuse, Vec2i(0, 256), Vec2i(128, 128));
		Debug::RenderTexture(brickNormal, Vec2i(0, 128), Vec2i(128, 128));
		Debug::RenderTexture(brickDisplacement, Vec2i(0, 0), Vec2i(128, 128));

		Debug::Write("FPS: " + String::Convert(FPS), ColorRGBA(255, 255, 255, 255), Vec2f(10, -10), Align::TopLeft, charmap);
		Debug::Write("Tesselation: " + String::Convert(tessellationAmount), ColorRGBA(255, 255, 255, 255), Vec2f(10, -30), Align::TopLeft, charmap);
		 
		Renderer::UpdateTarget();
	} 

	void Cleanup() override
	{
	}	

	void ConsoleMode()
	{
		String input;

		Console::Write("Enter command: ");
		 
		Console::WaitForInput();
		Console::ReadWord(input);

		if (input == "set")
		{ 
			Console::WaitForInput(); 
			Console::ReadWord(input);

			if (input == "rotation")
			{
				Vec3f rot;
				Console::Write("Enter (x, y, z) rotation: ");
				Console::WaitForInput();
				Console::ReadWord(input);
				if (String::ConvertTo(input, rot.x) == false)
					Console::Write("Failed reading input, exiting console mode\n");
				if (String::ConvertTo(input, rot.y) == false)
					Console::Write("Failed reading input, exiting console mode\n");
				if (String::ConvertTo(input, rot.z) == false)
					Console::Write("Failed reading input, exiting console mode\n");

				transform.rot =
					Quatf(AxisAnglef(Vec3f(0, 0, 1), Math::Radians(rot.z))) *
					Quatf(AxisAnglef(Vec3f(0, 1, 0), Math::Radians(rot.y))) *
					Quatf(AxisAnglef(Vec3f(1, 0, 0), Math::Radians(rot.x)));
			}
		}
	}
			
	InputEventFunction<InputEventType::WindowClosed> WindowClosed = [&](Window&) {
		Stop();
	};
	InputEventFunction<InputEventType::WindowResized> WindowResized = [&](int x, int y, Window&)
	{
		Renderer::SetViewport(Vec2i(), Vec2i(x, y));

		proj = Math::PerspectiveMatrix<float>(90, Renderer::GetViewportRatio(), 0.1, 10);
	};
};