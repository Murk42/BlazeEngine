#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

#include "DynamicMatrix.h"


void CloseWindowEvent(Window* win);
void ResizeWindowEvent(int w, int h, Window* win);
 
struct MatProps : MaterialProperties<Mat4f, TextureArray2D>
{
	Property<Mat4f> mvp = "u_MVP";
	Property<TextureArray2D> texture = "u_texture";
};

class App : public Application<App>
{
public:
	Console::LogList logList = Console::LogList(Vec2i(0, 0), Vec2i(200, 20)); 
	Window window;	

	Material<MatProps> material;
	Mesh mesh;			

	TextureArray2D texture; 

	static constexpr int sizeX = 10, sizeY = 10;
	Vertex<Vec2f, Vec2f, float, float> vertices[sizeX * sizeY];		

	void Startup() override
	{		
		int offsetX = 5, offsetY = 5;
		for (int y = 0; y < sizeY; ++y)
		{
			for (int x = 0; x < sizeX; ++x)
			{
				vertices[x + y * sizeX].GetValue<0>() = Vec2i(offsetX, offsetY);
				vertices[x + y * sizeX].GetValue<1>() = Vec2i(offsetX + 100, offsetY + 100);
				vertices[x + y * sizeX].GetValue<2>() = (x + y) % 10;
				vertices[x + y * sizeX].GetValue<3>() = 0;

				offsetX += 105;
			}
			offsetX = 0;
			offsetY += 105;
		}

		Input::SetEventFunction(InputEvent::WindowClosed, CloseWindowEvent);
		Input::SetEventFunction(InputEvent::WindowResized, ResizeWindowEvent);

		window.SetSize(Vec2i(800, 800));
		window.SetWindowed(true, false);
		window.ShowWindow(true);
		
		Renderer::SetViewport(Vec2i(), window.GetSize());
		Renderer::SetTarget(window);

		{
			texture.Load("assets/sprites/sprites.png", Vec2i(16, 16));

			Shader vertexShader = Shader(ShaderType::VertexShader, "assets/shaders/sprite/vertex.glsl");
			Shader fragmentShader = Shader(ShaderType::FragmentShader, "assets/shaders/sprite/fragment.glsl");
			Shader geometryShader = Shader(ShaderType::GeometryShader, "assets/shaders/sprite/geometry.glsl");
			material.SetShaders(vertexShader, fragmentShader, geometryShader);

			material.properties.mvp = Math::OrthographicMatrix<float>(0, window.GetSize().x, 0, window.GetSize().y, -1, 1);
			material.properties.texture = &texture;

			mesh.SetVertices(vertices, sizeX * sizeY);
		}
	}

	void Frame() override
	{
		Input::Update();		

		//test2

		Renderer::ClearTarget();

		Renderer::RenderPointArray(material, mesh);

		Renderer::UpdateTarget();		

		ProcessLogs();
	}

	void ProcessLogs()
	{
		Log log;
		while (Logger::GetNextLog(log))
		{
			logList.AddLog(log);
		}

		logList.Refresh();
	}
};

void CloseWindowEvent(Window* win)
{
	App::Stop();
}

void ResizeWindowEvent(int w, int h, Window* win)
{
	App& app = App::Instance();

	app.material.properties.mvp = Math::OrthographicMatrix<float>(0, w, 0, h, -1, 1);
	Renderer::SetViewport(Vec2i(), Vec2i(w, h));
}