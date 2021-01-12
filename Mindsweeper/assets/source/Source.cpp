#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

void CloseWindowEvent(Window* win);
void ResizeWindowEvent(int w, int h, Window* win);
 
struct MatProps : MaterialProperties<Mat4f, TextureArray2D>
{	
	Property<Mat4f> mvp = "u_MVP";
	Property<TextureArray2D> texture = "u_texture";
};
struct TextMatProps : MaterialProperties<Mat4f, Vec4f, Texture2D>
{
	Property<Mat4f> mvp = "u_MVP";
	Property<Vec4f> color = "u_color";
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
	Console::LogList logList = Console::LogList(Vec2i(0, 0), Vec2i(100, 10)); 
	Console::Frame frame = Console::Frame(Vec2i(0, 11), Vec2i(100, 10));
	Window window;	

	Scene scene = Scene::Menu;

	Material<TextMatProps> textMaterial;
	Material<MatProps> material;
	Mesh mesh;			

	Font font;
	TextureArray2D texture; 
	   
	static constexpr int posX = 10, posY = 10;
	static constexpr int maxSizeX = 32, maxSizeY = 16;
	static constexpr int tileSizeX = 32, tileSizeY = 32;

	Vertex<Vec2f, Vec2f, float, float> vertices[maxSizeX * maxSizeY];	
	int valueMatrix[maxSizeX * maxSizeY];
	int textureMatrix[maxSizeX * maxSizeY];
	bool checkedMatrix[maxSizeX * maxSizeY];

	int sizeX =16 , sizeY = 8, mineCount=10;
	 
	Mat4f canvasProjection;
	Mat4f tilesTrans;
	 
	struct {  
		NormalText menuTitle;
		Mat4f menuTitleTrans;

		NormalText title;
		Mat4f titleTrans;
		NormalText details;
		Mat4f detailsTrans;

	} text;

	void Startup() override
	{				
		memset(vertices, 0, sizeof(vertices));

		int offsetX = 0, offsetY = 0;
		for (int y = 0; y < sizeY; ++y)
		{
			for (int x = 0; x < sizeX; ++x)
			{
				vertices[x + y * maxSizeX].GetValue<0>() = Vec2i(offsetX, offsetY);
				vertices[x + y * maxSizeX].GetValue<1>() = Vec2i(offsetX + tileSizeX, offsetY + tileSizeY);				
				vertices[x + y * maxSizeX].GetValue<2>() = 1;
				vertices[x + y * maxSizeX].GetValue<3>() = 0;

				textureMatrix[x + y * maxSizeX] = 13;

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

		GenerateTiles();
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
				Shader vertexShader = Shader(ShaderType::VertexShader, "assets/default/shaders/normalText/vertex.glsl");
				Shader fragmentShader = Shader(ShaderType::FragmentShader, "assets/default/shaders/normalText/fragment.glsl");
				Shader geometryShader = Shader(ShaderType::GeometryShader, "assets/default/shaders/normalText/geometry.glsl");
				textMaterial.SetShaders(vertexShader, fragmentShader, geometryShader);
			}			
			mesh.SetVertices(vertices, maxSizeX * maxSizeY);

			text.title.SetFont(&font, 50);
			text.title.SetString("Minesweeper");			

			text.details.SetFont(&font, 20); 
			text.details.SetString(String(format_string, "Size is %dx%d", maxSizeX, maxSizeY));
			 
			text.menuTitle.SetFont(&font, 100);
			text.menuTitle.SetString("Minesweeper");
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
			textMaterial.properties.color = Color(255, 128, 0).ToVector();
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
				mp -= Vec2i(posX, posY);
				mp /= Vec2i(tileSizeX, tileSizeY);

				if (mp.x >= 0 && mp.x < sizeX && mp.y >= 0 && mp.y < sizeY)
				{
					int Checker = ClickOnTile(mp.x, mp.y);
					switch (Checker)
					{
					case 1:
						text.details.SetString(String("Booom!"));
						textureMatrix[mp.x + mp.y * maxSizeX] = 1;
						for (int y = 0; y < sizeX; ++y)
						{
							for (int x = 0; x < sizeX; ++x)
							{
								int k = x + y * maxSizeX;
								if (textureMatrix[k] == 13 && valueMatrix[k] == 1)
								{
									textureMatrix[k] = 2;
								}
								if (textureMatrix[k] == 11 && valueMatrix[k] == 0)
								{
									textureMatrix[k] = 0;
								}
							}
						}
						break;
					}
					memset(checkedMatrix, 0, sizeof(checkedMatrix));
					UpdateTiles();
				}
			}
			
			if (Input::GetKeyState(Key::MouseRight) == KeyState::Pressed)
			{
				Vec2i mp = Input::GetMousePos();
				mp.y = window.GetSize().y - mp.y;
				mp -= Vec2i(posX, posY);
				mp /= Vec2i(tileSizeX, tileSizeY);

				if (mp.x >= 0 && mp.x < sizeX && mp.y >= 0 && mp.y < sizeY)
				{
					
					switch (textureMatrix[mp.x + mp.y * maxSizeX])
					{
					case 13:
						textureMatrix[mp.x + mp.y * maxSizeX] = 11;
						mineCount--;
						break;
					case 11:
						textureMatrix[mp.x + mp.y * maxSizeX] = 13;
						mineCount++;
						break;
					default:
						break;
					}
					UpdateTiles();
				}
			}

			material.properties.mvp = canvasProjection * tilesTrans;
			material.properties.texture = &texture;
			Renderer::RenderPointArray(material, mesh.vl);

			textMaterial.properties.mvp = canvasProjection * text.titleTrans;
			textMaterial.properties.texture = text.title.GetTexture();
			textMaterial.properties.color = Color(255, 128, 0).ToVector();
			Renderer::RenderPointArray(textMaterial, text.title.GetMesh());

			textMaterial.properties.mvp = canvasProjection * text.detailsTrans;
			textMaterial.properties.texture = text.details.GetTexture();
			textMaterial.properties.color = Color(255).ToVector();
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

	void GenerateTiles( )
	{
		if (mineCount > sizeX * sizeY)
		{
			Logger::AddLog(LogType::Message, __FUNCTION__, "The number of mines is bigger than number of tiles!");
		}

		srand(time(0));
		for (int i = 0; i < mineCount; ++i) 
		{
			int x1 = Random::Float(0, sizeX);
			int y1 = Random::Float(0, sizeY);
			int k = x1 + maxSizeX * y1;
			bool twoSquare = 1;
			bool threeSquare = 1;
			if (valueMatrix[k] == 1)
			{
				i--;
				continue;
			}
			valueMatrix[k] = 1;
			if (valueMatrix[k - 1] == 0) 
			{
				threeSquare = 0;
				continue;
			}
			if (valueMatrix[k + 1] == 0) 
			{
				threeSquare = 0;
				twoSquare = 0;
				continue;
			}
			if (valueMatrix[k - maxSizeX] == 0)
			{
				threeSquare = 0;
				continue;
			}
			if (valueMatrix[k - maxSizeX + 1] == 0) 
			{
				threeSquare = 0;
				continue;
			}
			if (valueMatrix[k - maxSizeX - 1] == 0) 
			{
				threeSquare = 0;
				continue;
			}
			if (valueMatrix[k + maxSizeX] == 0)
			{
				threeSquare = 0;
				twoSquare = 0;
				continue;
			}
			if (valueMatrix[k + maxSizeX + 1] == 0)
			{
				threeSquare = 0;
				twoSquare = 0;
				continue;
			}
			if (valueMatrix[k + maxSizeX - 1] == 0)
			{
				threeSquare = 0;
				continue;
			}
			if (twoSquare)
			{
				i--;
				valueMatrix[k + maxSizeX] = 0;
			}
			if (threeSquare)
			{
				i -= 2;
				valueMatrix[k - maxSizeX] = 0;
				valueMatrix[k + maxSizeX + 1] = 0;
			}
		}


		UpdateTiles();
	}

	void UpdateTiles()
	{
		Logger::AddLog(LogType::Message, __FUNCTION__, String(format_string, "Number of mines: %d", mineCount));
		if (mineCount == 0) 
		{
			bool isMineOpened = 1;
			for (int y = 0; y < sizeY; ++y)
			{					
				for (int x = 0; x < sizeX; ++x)
				{
					if (textureMatrix[x + y * maxSizeX] == 13)
					{							
						isMineOpened = 0;
						break;
					}
				}
			}
			if (isMineOpened) text.details.SetString(String("You won!"));
		}
		for (int y = 0; y < sizeY; ++y)
		{
			for (int x = 0; x < sizeX; ++x)
			{
				vertices[x + maxSizeX * y].GetValue<2>() = textureMatrix[x + maxSizeX * y];
			}
		}

		mesh.ChangeVertices(vertices, maxSizeX * maxSizeY, 0);
	}

	bool inRange(int x, int y) 
	{
		if (x >= 0 && x < sizeX && y >= 0 && y < sizeY)
			return 1;
		else 
			return 0;
	}

	int ClickOnTile(int x, int y)
	{
		int k = x + y * maxSizeX;
		if (valueMatrix[k] == 1)
		{
			Logger::AddLog(LogType::Message, __FUNCTION__, "Ops, you hit bomb!");
			return 1;
		}
		if (checkedMatrix[k]) 
		{
			return 0;
		}
		checkedMatrix[k] = 1;
		
		int NumberOfMines = 0;
		if (inRange(x - 1, y) && valueMatrix[k - 1] == 1) NumberOfMines++;
		if (inRange(x + 1, y) && valueMatrix[k + 1] == 1) NumberOfMines++;
		if (inRange(x, y - 1) && valueMatrix[k - maxSizeX] == 1) NumberOfMines++;
		if (inRange(x - 1, y - 1) && valueMatrix[k - maxSizeX - 1] == 1) NumberOfMines++;
		if (inRange(x + 1, y - 1) && valueMatrix[k - maxSizeX + 1] == 1) NumberOfMines++;
		if (inRange(x, y + 1) && valueMatrix[k + maxSizeX] == 1) NumberOfMines++;
		if (inRange(x - 1, y + 1) && valueMatrix[k + maxSizeX - 1] == 1) NumberOfMines++;
		if (inRange(x + 1, y + 1) && valueMatrix[k + maxSizeX + 1] == 1) NumberOfMines++;

		switch (NumberOfMines)
		{
		case 0:
			textureMatrix[k] = 12;
			if (inRange(x - 1, y))
			{
				ClickOnTile(x - 1, y);
			}
			if (inRange(x + 1, y))
			{
				ClickOnTile(x + 1, y);
			}
			if (inRange(x, y - 1))
			{
				ClickOnTile(x, y - 1);
			}
			if (inRange(x - 1, y - 1))
			{
				ClickOnTile(x - 1, y - 1);
			}
			if (inRange(x + 1, y - 1))
			{
				ClickOnTile(x + 1, y - 1);
			}
			if (inRange(x, y + 1))
			{
				ClickOnTile(x, y + 1);
			}
			if (inRange(x - 1, y + 1))
			{
				ClickOnTile(x - 1, y + 1);
			}
			if (inRange(x + 1, y + 1))
			{
				ClickOnTile(x + 1, y + 1);
			}
			break;
		case 1:
			textureMatrix[k] = 10;			
			break;
		case 2:
			textureMatrix[k] = 9;			
			break;
		case 3:
			textureMatrix[k] = 8;			
			break;
		case 4:
			textureMatrix[k] = 7;			
			break;
		case 5:
			textureMatrix[k] = 6;			
			break;
		case 6:
			textureMatrix[k] = 5;			
			break;
		case 7:
			textureMatrix[k] = 4;			
			break;
		case 8:
			textureMatrix[k] = 3;			
			break;
		}
		
		return 0;
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
		app.tilesTrans = Math::TranslationMatrix<float>(Vec2i(App::posX, App::posY));
		app.text.titleTrans = Math::TranslationMatrix<float>(Vec2i(10, h - 5 - app.text.title.GetSize().y));
		app.text.detailsTrans = app.text.titleTrans * Math::TranslationMatrix<float>(Vec2i(app.text.title.GetSize().x + 30, 0));
		break;
	}
	}

	app.canvasProjection = Math::OrthographicMatrix<float>(0, w, 0, h, -1, 1);	
	Renderer::SetViewport(Vec2i(), Vec2i(w, h));
}