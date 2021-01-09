#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

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

	static constexpr int sizeX = 32, sizeY = 16;
	static constexpr int tileSizeX = 32, tileSizeY = 32;
	Vertex<Vec2f, Vec2f, float, float> vertices[sizeX * sizeY];	

	Mat4f canvasProjection;
	Mat4f tilesTrans;

	void Startup() override
	{				
		int offsetX = 0, offsetY = 0;
		for (int y = 0; y < sizeY; ++y)
		{
			for (int x = 0; x < sizeX; ++x)
			{
				vertices[x + y * sizeX].GetValue<0>() = Vec2i(offsetX, offsetY);
				vertices[x + y * sizeX].GetValue<1>() = Vec2i(offsetX + tileSizeX, offsetY + tileSizeY);
				vertices[x + y * sizeX].GetValue<2>() = 9;
				vertices[x + y * sizeX].GetValue<3>() = 0;

				offsetX += tileSizeX;
			}
			offsetX = 0;
			offsetY += tileSizeY;
		}

		Input::SetEventFunction(InputEvent::WindowClosed, CloseWindowEvent);
		Input::SetEventFunction(InputEvent::WindowSizeChanged, ResizeWindowEvent);

		window.SetSize(Vec2i(tileSizeX * sizeX, tileSizeY * sizeY));
		window.SetWindowed(true, false);
		window.ShowWindow(true);
		
		Renderer::SetViewport(Vec2i(), window.GetSize());
		Renderer::SetTarget(window);

		//Loading files
		{
			texture.Load("assets/sprites/sprites.png", Vec2i(16, 16));

			Shader vertexShader = Shader(ShaderType::VertexShader, "assets/shaders/sprite/vertex.glsl");
			Shader fragmentShader = Shader(ShaderType::FragmentShader, "assets/shaders/sprite/fragment.glsl");
			Shader geometryShader = Shader(ShaderType::GeometryShader, "assets/shaders/sprite/geometry.glsl");
			material.SetShaders(vertexShader, fragmentShader, geometryShader);


			mesh.SetVertices(vertices, sizeX * sizeY);
		}		
		
	}

	void Frame() override 
	{
		Input::Update();				

		Renderer::ClearTarget();

		if (Input::GetKeyState(Key::MouseLeft) == KeyState::Pressed)
		{ 
			Vec2i mp = Input::GetMousePos();
			if (mp.x > (window.GetSize().x - sizeX * tileSizeX) / 2 &&
				mp.y > (window.GetSize().y - sizeY * tileSizeY) / 2)
			{
				mp -= window.GetSize() - Vec2i(sizeX * tileSizeX, sizeY * tileSizeY);
				mp /= Vec2i(tileSizeX, tileSizeY);
				mp.y = sizeY - mp.y - 1;
				vertices[mp.x + sizeX * mp.y].GetValue<2>() = ((uint)vertices[mp.x + sizeX * mp.y].GetValue<2>() + 1) % 10;

				mesh.ChangeVertices(vertices, sizeX * sizeY, 0);
			}
		}
			
		material.properties.mvp = canvasProjection * tilesTrans;
		material.properties.texture = &texture;
		Renderer::RenderPointArray(material, mesh.vl);

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

 	app.tilesTrans = Math::TranslationMatrix<float>(Vec2i(w - App::tileSizeX * App::sizeX, h - App::tileSizeY * App::sizeY) / 2);

	app.canvasProjection = Math::OrthographicMatrix<float>(0, w, 0, h, -1, 1);	
	Renderer::SetViewport(Vec2i(), Vec2i(w, h));
}