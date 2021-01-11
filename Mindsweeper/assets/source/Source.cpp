#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

void CloseWindowEvent(Window* win);
void ResizeWindowEvent(int w, int h, Window* win);
 
struct MatProps : MaterialProperties<Mat4f, TextureArray2D>
{	
	Property<Mat4f> mvp = "u_MVP";
	Property<TextureArray2D> texture = "u_texture";
};
struct TextMatProps : MaterialProperties<Mat4f, Texture2D>
{
	Property<Mat4f> mvp = "u_MVP";
	Property<Texture2D> texture = "u_texture";
};

enum class Scene
{
	Menu,
	Game,
};
   
class App : public Application<App>
{   
public: 
	Console::LogList logList = Console::LogList(Vec2i(0, 0), Vec2i(200, 20)); 
	Window window;	

	Scene scene = Scene::Menu;

	Material<TextMatProps> textMaterial;
	Material<MatProps> material;
	Mesh mesh;			

	Font font;
	TextureArray2D texture; 
	   
	static constexpr int maxPosX = 10, maxPosY = 10;
	static constexpr int maxSizeX = 16, maxSizeY = 8;
	static constexpr int tileSizeX = 64, tileSizeY = 64;

	Vertex<Vec2f, Vec2f, float, float> vertices[maxSizeX * maxSizeY];	
	int matrix[maxSizeX * maxSizeY];

	int sizeX, sizeY;
	 
	Mat4f canvasProjection;
	Mat4f tilesTrans;
	 
	struct {  
		TextRenderer menuTitle;
		Mat4f menuTitleTrans;

		TextRenderer title;
		Mat4f titleTrans;
		TextRenderer details;
		Mat4f detailsTrans;

	} text;

	void Startup() override
	{				
		int offsetX = 0, offsetY = 0;
		for (int y = 0; y < maxSizeY; ++y)
		{
			for (int x = 0; x < maxSizeX; ++x)
			{
				vertices[x + y * maxSizeX].GetValue<0>() = Vec2i(offsetX, offsetY);
				vertices[x + y * maxSizeX].GetValue<1>() = Vec2i(offsetX + tileSizeX, offsetY + tileSizeY);
				vertices[x + y * maxSizeX].GetValue<2>() = 13;
				vertices[x + y * maxSizeX].GetValue<3>() = 0;

				offsetX += tileSizeX;
			}
			offsetX = 0;
			offsetY += tileSizeY;
		}

		Input::SetEventFunction(InputEvent::WindowClosed, CloseWindowEvent);
		Input::SetEventFunction(InputEvent::WindowSizeChanged, ResizeWindowEvent);

		window.SetSize(Vec2i(10 + tileSizeX * maxSizeX + 10, 10 + tileSizeY * maxSizeY + 10 + 50 + 10));
		window.SetWindowed(true, false);
		window.ShowWindow(true);
		
		Renderer::SetClearColor(Color(160));
		Renderer::SetViewport(Vec2i(), window.GetSize());
		Renderer::SetTarget(window);
		Renderer::UseBlending(true);

		//Loading filesx
		{
			font.Load("assets/fonts/Roboto-Regular.ttf");

			texture.Load("assets/sprites/SpriteSheet.png", Vec2i(64, 64));

			{
				Shader vertexShader = Shader(ShaderType::VertexShader, "assets/shaders/sprite/vertex.glsl");
				Shader fragmentShader = Shader(ShaderType::FragmentShader, "assets/shaders/sprite/fragment.glsl");
				Shader geometryShader = Shader(ShaderType::GeometryShader, "assets/shaders/sprite/geometry.glsl");
				material.SetShaders(vertexShader, fragmentShader, geometryShader);
			} 

			{   
				Shader vertexShader = Shader(ShaderType::VertexShader, "assets/shaders/text/vertex.glsl");
				Shader fragmentShader = Shader(ShaderType::FragmentShader, "assets/shaders/text/fragment.glsl");
				Shader geometryShader = Shader(ShaderType::GeometryShader, "assets/shaders/text/geometry.glsl");
				textMaterial.SetShaders(vertexShader, fragmentShader, geometryShader);
			}			
			mesh.SetVertices(vertices, maxSizeX * maxSizeY);

			text.title.SetFont(&font, 50);
			text.title.SetString("Minesweeper");
			text.title.SetColors({
					Color(148, 0, 211),
					Color(75, 0, 130),
					Color(0, 0, 255),
					Color(0, 255, 0),
					Color(255, 255, 0),
					Color(255, 127, 0),
					Color(255, 0, 0),
					Color(148, 0, 211),
					Color(75, 0, 130),
					Color(0, 0, 255),
					Color(0, 255, 0),
				});

			text.details.SetFont(&font, 20);
			text.details.SetString(String(format_string, "Size is %dx%d", maxSizeX, maxSizeY));

			text.menuTitle.SetFont(&font, 100);
			text.menuTitle.SetString("Minesweeper");
			text.menuTitle.SetColors({
					Color(148, 0, 211),
					Color(75, 0, 130),
					Color(0, 0, 255),
					Color(0, 255, 0),
					Color(255, 255, 0),
					Color(255, 127, 0),
					Color(255, 0, 0),
					Color(148, 0, 211),
					Color(75, 0, 130),
					Color(0, 0, 255),
					Color(0, 255, 0),
				});
		}				
	}

	void Frame() override 
	{
		Input::Update();				

		Renderer::ClearTarget();

		switch (scene)
		{
		case Scene::Menu: {
			textMaterial.properties.mvp = canvasProjection * text.menuTitleTrans;
			textMaterial.properties.texture = text.menuTitle.GetTexture();
			Renderer::RenderPointArray(textMaterial, text.menuTitle.GetMesh());

			if (Input::GetKeyState(Key::MouseLeft) == KeyState::Down)
				ChangeToGameScene();
			break;
		}
		case Scene::Game: {
			if (Input::GetKeyState(Key::MouseLeft) == KeyState::Pressed)
			{
				Vec2i mp = Input::GetMousePos();
				mp.y = window.GetSize().y - mp.y;
				mp -= Vec2i(maxPosX, maxPosY);
				mp /= Vec2i(tileSizeX, tileSizeY);

				if (mp.x >= 0 &&
					mp.x < maxSizeX &&
					mp.y >= 0 &&
					mp.y < maxSizeY)
				{
					vertices[mp.x + maxSizeX * mp.y].GetValue<2>() = ((uint)vertices[mp.x + maxSizeX * mp.y].GetValue<2>() + 1) % 14;

					mesh.ChangeVertices(vertices, maxSizeX * maxSizeY, 0);
				}
			}

			material.properties.mvp = canvasProjection * tilesTrans;
			material.properties.texture = &texture;
			Renderer::RenderPointArray(material, mesh.vl);

			textMaterial.properties.mvp = canvasProjection * text.titleTrans;
			textMaterial.properties.texture = text.title.GetTexture();
			Renderer::RenderPointArray(textMaterial, text.title.GetMesh());

			textMaterial.properties.mvp = canvasProjection * text.detailsTrans;
			textMaterial.properties.texture = text.details.GetTexture();
			Renderer::RenderPointArray(textMaterial, text.details.GetMesh());
			break;
		}
		}

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

	void ChangeToMenuScene()
	{
		scene = Scene::Menu;
		ResizeWindowEvent(window.GetSize().x, window.GetSize().y, &window);
	}

	void ChangeToGameScene() 
	{
		scene = Scene::Game;
		ResizeWindowEvent(window.GetSize().x, window.GetSize().y, &window);
	}	

	void UpdateTiles()
	{
		for (int y = 0; y < sizeY; ++y)
			for (int x = 0; x < sizeX; ++x)			
				vertices[x + maxSizeX * y].GetValue<2>() = matrix[x + maxSizeX * y];

		mesh.ChangeVertices(vertices, maxSizeX * maxSizeY, 0);
	}

	void GenetateTiles()
	{

	}

	void ClickOnTile(int x, int y)
	{

	}
};

void CloseWindowEvent(Window* win)
{
	App::Stop();
}
void ResizeWindowEvent(int w, int h, Window* win)
{	
	App& app = App::Instance();	

	switch (app.scene)
	{
	case Scene::Menu : {
		app.text.menuTitleTrans = Math::TranslationMatrix<float>(Vec2i((w - app.text.menuTitle.GetSize().x) / 2, h - app.text.menuTitle.GetSize().y - 50));
		break;
		}
	case Scene::Game: {
		app.tilesTrans = Math::TranslationMatrix<float>(Vec2i(App::maxPosX, App::maxPosY));
		app.text.titleTrans = Math::TranslationMatrix<float>(Vec2i(10, h - 5 - app.text.title.GetSize().y));
		app.text.detailsTrans = app.text.titleTrans * Math::TranslationMatrix<float>(Vec2i(app.text.title.GetSize().x + 30, 0));
		break;
	}
	}

	app.canvasProjection = Math::OrthographicMatrix<float>(0, w, 0, h, -1, 1);	
	Renderer::SetViewport(Vec2i(), Vec2i(w, h));
}