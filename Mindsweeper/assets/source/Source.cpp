#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

void CloseWindowEvent(Window* win);
void ResizeWindowEvent(int w, int h, Window* win);
void SetUp();
 
struct TilesMatProps : MaterialProperties<Mat4f, TextureArray2D>
{	
	Property<Mat4f> mvp = "u_MVP";
	Property<TextureArray2D> texture = "u_texture";
};
struct TextMatProps : MaterialProperties<Mat4f, Texture2D, Vec4f>
{
	Property<Mat4f> mvp = "u_MVP"; 
	Property<Texture2D> texture = "u_texture";
	Property<Vec4f> color = "u_color";
};
struct ButtonMatProps : MaterialProperties<Mat4f, TextureArray2D>
{
	Property<Mat4f> mvp = "u_MVP";
	Property<TextureArray2D> texture = "u_texture";
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

	Material<TilesMatProps> tilesMaterial;
	Material<TextMatProps> textMaterial;
	Material<ButtonMatProps> buttonMaterial;

	Font font;
	TextureArray2D tilesSpriteSheet;
	TextureArray2D buttonTexture;

	Mesh tilesMesh;			
	    
	static constexpr int posX = 10, posY = 10;
	static constexpr int maxSizeX = 100, maxSizeY = 50;
	static constexpr int tileSizeX = 32, tileSizeY = 32;

	Vertex<Vec2f, Vec2f, float, float> vertices[maxSizeX * maxSizeY];	
	int valueMatrix[maxSizeX * maxSizeY];
	int textureMatrix[maxSizeX * maxSizeY];
	bool checkedMatrix[maxSizeX * maxSizeY];

	int sizeX = 20, sizeY = 8, startMineCount = 10;
	int mineCount = startMineCount;

	bool gameEnded = 0;
	 
	Mat4f canvasProjection;
	Mat4f tilesTrans;

	struct {		
		NormalText titleText;
		Mat4f titleTextTrans;

		Button playButton;
		Mat4f playButtonTrans;
	} menu;
	 
	struct {		
		Button restartButton;

		NormalText titleText;
		Mat4f titleTextTrans;
		NormalText detailsText;
		Mat4f detailsTextTrans;
	} game;	

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

		window.SetSize(Vec2i(10 + tileSizeX * sizeX + 10, 10 + tileSizeY * sizeY + 10 + 50 + 10));
		window.SetWindowed(true, false);
		window.ShowWindow(true);

		Renderer::SetClearColor(Color(160));
		Renderer::SetViewport(Vec2i(), window.GetSize());
		Renderer::SetTarget(window);
		Renderer::UseBlending(true);

		font.Load("assets/fonts/Roboto-Regular.ttf");

		//Load tilesMaterial 
		{
			Shader vertexShader = Shader(ShaderType::VertexShader, "assets/shaders/sprite/vertex.glsl");
			Shader fragmentShader = Shader(ShaderType::FragmentShader, "assets/shaders/sprite/fragment.glsl");
			Shader geometryShader = Shader(ShaderType::GeometryShader, "assets/shaders/sprite/geometry.glsl");
			tilesMaterial.SetShaders(vertexShader, fragmentShader, geometryShader);
		}

		//Load textMaterial
		{
			Shader vertexShader = Shader(ShaderType::VertexShader, "assets/default/shaders/normalText/vertex.glsl");
			Shader fragmentShader = Shader(ShaderType::FragmentShader, "assets/default/shaders/normalText/fragment.glsl");
			Shader geometryShader = Shader(ShaderType::GeometryShader, "assets/default/shaders/normalText/geometry.glsl");
			textMaterial.SetShaders(vertexShader, fragmentShader, geometryShader);
		}

		//Load ButtonMaterial 
		{
			Shader vertexShader = Shader(ShaderType::VertexShader, "assets/default/shaders/button/vertex.glsl");
			Shader fragmentShader = Shader(ShaderType::FragmentShader, "assets/default/shaders/button/fragment.glsl");
			Shader geometryShader = Shader(ShaderType::GeometryShader, "assets/default/shaders/button/geometry.glsl");
			buttonMaterial.SetShaders(vertexShader, fragmentShader, geometryShader);
		}

		//Seting up menu scene 
		{
			buttonTexture.Load("assets/sprites/buttonTexture.png", Vec2i(48, 48));

			menu.titleText.SetFont(&font, 100);
			menu.titleText.SetString("Minesweeper");

			menu.playButton.cornerSize = Vec2u(20);			
		}

		//Seting up game scene
		{
			tilesSpriteSheet.Load("assets/sprites/SpriteSheet.png", Vec2i(64, 64));
			tilesSpriteSheet.SetSettings(TextureSampling::Linear, TextureSampling::Linear);
			tilesMesh.SetVertices(vertices, maxSizeX * maxSizeY);

			game.titleText.SetFont(&font, 50);
			game.titleText.SetString("Minesweeper");

			game.restartButton.cornerSize = Vec2u(20, 20);

			game.detailsText.SetFont(&font, 20);
			game.detailsText.SetString(String(format_string, "Size is %dx%d", sizeX, sizeY));
		}
	}

	void Frame() override 
	{
		Input::Update();				
		 
		Renderer::ClearTarget(); 
		 
		switch (scene)
		{
		case Scene::Menu: { 

			menu.playButton.Update();
			menu.playButton.UpdateMesh();

			//Render menuTitle
			{ 
				buttonMaterial.properties.mvp = canvasProjection;
				buttonMaterial.properties.texture = &buttonTexture;
				Renderer::RenderPointArray(buttonMaterial, menu.playButton.GetMesh());  

				textMaterial.properties.mvp = canvasProjection * menu.titleTextTrans;
				textMaterial.properties.texture = menu.titleText.GetTexture();
				textMaterial.properties.color = Color(255, 128, 0).ToVector(); 
				Renderer::RenderPointArray(textMaterial, menu.titleText.GetMesh());
			}			

			if (menu.playButton.GetState() == ButtonState::Down)
			{
				//Logger::AddLog(LogType::Message, __FUNCTION__, "pusikaronja");
				ChangeToGameScene();
			}
			break;
		}
		case Scene::Game: {
			game.restartButton.Update();
			game.restartButton.UpdateMesh();
			
			if (game.restartButton.GetState() == ButtonState::Pressed)
				ChangeToGameScene();

			if (Input::GetKeyState(Key::MouseLeft) == KeyState::Pressed && !gameEnded)
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
					case 0:
						break;
					case 1:
						game.detailsText.SetString(String("Booooom!"));
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
						gameEnded = 1;
						break;
					}
					memset(checkedMatrix, 0, sizeof(checkedMatrix));
					UpdateTiles();
				}
			}
			
			if (Input::GetKeyState(Key::MouseRight) == KeyState::Pressed && !gameEnded)
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

			buttonMaterial.properties.mvp = canvasProjection;
			buttonMaterial.properties.texture = &buttonTexture;
			Renderer::RenderPointArray(buttonMaterial, game.restartButton.GetMesh());

			tilesMaterial.properties.mvp = canvasProjection * tilesTrans;
			tilesMaterial.properties.texture = &tilesSpriteSheet;
			Renderer::RenderPointArray(tilesMaterial, tilesMesh.vl);

			textMaterial.properties.mvp = canvasProjection * game.titleTextTrans;
			textMaterial.properties.texture = game.titleText.GetTexture();
			textMaterial.properties.color = Color(255, 128, 0).ToVector();
			Renderer::RenderPointArray(textMaterial, game.titleText.GetMesh());

			textMaterial.properties.mvp = canvasProjection * game.detailsTextTrans;
			textMaterial.properties.texture = game.detailsText.GetTexture();
			textMaterial.properties.color = Color(255).ToVector();
			Renderer::RenderPointArray(textMaterial, game.detailsText.GetMesh());	
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

		memset(valueMatrix, 0, sizeof(valueMatrix));
		memset(checkedMatrix, 0, sizeof(checkedMatrix));
		mineCount = startMineCount;
		gameEnded = 0;
		GenerateTiles();		
	}	

	void GenerateTiles( )
	{
		if (mineCount > sizeX * sizeY)
		{
			Logger::AddLog(LogType::Message, __FUNCTION__, "The number of mines is bigger than number of tiles!");
		}
		for (int y = 0; y < sizeY; ++y)
		{
			for (int x = 0; x < sizeX; ++x)
			{
				int k = x + y * maxSizeX;
				textureMatrix[k] = 13;
			}
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
					Logger::AddLog(LogType::Message, __FUNCTION__, String(format_string, "uso: %d", mineCount));
					if (textureMatrix[x + y * maxSizeX] == 13)
					{							
						isMineOpened = 0;
						break;
					}
					
				}
			}
			if (isMineOpened)
			{
				game.detailsText.SetString(String("You won!"));
				gameEnded = 1;
			}
		}
		for (int y = 0; y < sizeY; ++y)
		{
			for (int x = 0; x < sizeX; ++x)
			{
				vertices[x + maxSizeX * y].GetValue<2>() = textureMatrix[x + maxSizeX * y];
			}
		}

		tilesMesh.ChangeVertices(vertices, maxSizeX * maxSizeY, 0);
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
		if (textureMatrix[k] == 11)
		{
			return 0;
		}
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
		app.menu.titleTextTrans = Math::TranslationMatrix<float>(Vec2i((w - app.menu.titleText.GetSize().x) / 2, h - app.menu.titleText.GetSize().y - 50));
		app.menu.playButton.rect = Rectu(Vec2u(w / 2 - 100, h - 270), Vec2u(200, 70));
		break;
		}
	case Scene::Game: {
		app.tilesTrans = Math::TranslationMatrix<float>(Vec2i(App::posX, App::posY));
		app.game.titleTextTrans = Math::TranslationMatrix<float>(Vec2i(10, h - 5 - app.game.titleText.GetSize().y));
		app.game.detailsTextTrans = app.game.titleTextTrans * Math::TranslationMatrix<float>(Vec2i(app.game.titleText.GetSize().x + 30, 0));
		app.game.restartButton.rect = Rectu(Vec2u(500, h - 65), Vec2u(120, 50));
		break;
	}
	}

	app.canvasProjection = Math::OrthographicMatrix<float>(0, w, 0, h, -1, 1);	
	Renderer::SetViewport(Vec2i(), Vec2i(w, h));
}