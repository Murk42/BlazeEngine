#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

void CloseWindowEvent(Window* win);
void ResizeWindowEvent(int w, int h, Window* win);
 
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
struct ButtonMatProps : MaterialProperties<Mat4f, TextureArray2D, float>
{
	Property<Mat4f> mvp = "u_MVP";
	Property<TextureArray2D> texture = "u_texture";
	Property<float> texScale = "u_texScale";
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

	int sizeX = 20, sizeY = 8, startMineCount = 20;
	int mineCount = startMineCount;

	bool gameEnded = 0;
	bool startedGame;
	 
	Mat4f canvasProjection;
	Mat4f tilesTrans;

	struct {		
		Button playButton;
		Mat4f playButtonTrans;
		NormalText playButtonText;
		Mat4f playButtonTextTrans;
		Color playButtonTextColor;

		NormalText titleText;
		Mat4f titleTextTrans;
	} menu;
	 
	struct {		
		Button restartButton;
		Mat4f restartButtonTrans;
		NormalText restartButtonText;
		Mat4f restartButtonTextTrans;
		Color restartButtonTextColor;

		NormalText titleText;
		Mat4f titleTextTrans;
		NormalText detailsText;
		Mat4f detailsTextTrans;
		NormalText timerText;
		Mat4f timerTextTrans;
		Timer timer;
		NormalText bombCounter;
		Mat4f bombCounterTrans;
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

		window.SetSize(Vec2i(800, 400));
		window.SetWindowed(false, false);
		window.ShowWindow(true);

		Renderer::SetClearColor(Color(100, 100, 120));
		Renderer::SetViewport(Vec2i(), window.GetSize());
		Renderer::SetTarget(window);
		Renderer::UseBlending(true);

		font.Load("assets/fonts/Pixellari.ttf");
		buttonTexture.Load("assets/sprites/ButtonSpriteSheet.png", Vec2i(96, 96));
		buttonTexture.SetSettings(TextureSampling::Linear, TextureSampling::Nearest);

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

			buttonMaterial.properties.texScale = 0.5f;
		}     
		      
		//Seting up menu scene   
		{   
			menu.titleText.SetFont(&font, 70);
			menu.titleText.SetString("Minesweeper");

			menu.playButton.textureCenterRect = Recti(26, 37, 44, 14);			
			menu.playButtonText.SetFont(&font, 30);
			menu.playButtonText.SetString("Play");
			menu.playButtonTextColor = Color(255);


			/* 
			* 
				26 + 44 + 26 = 96
			37
			+
			14
			+
			45
			=
			96

			*/
		}

		//Seting up game scene
		{
			tilesSpriteSheet.Load("assets/sprites/SpriteSheet.png", Vec2i(64, 64));
			tilesSpriteSheet.SetSettings(TextureSampling::Linear, TextureSampling::Linear);
			tilesMesh.SetVertices(vertices, maxSizeX * maxSizeY);

			game.titleText.SetFont(&font, 70);
			game.titleText.SetString("Minesweeper");

			game.detailsText.SetFont(&font, 20);
			game.detailsText.SetString(String(format_string, "Size is %dx%d", sizeX, sizeY));

			game.timerText.SetFont(&font, 20);			

			game.bombCounter.SetFont(&font, 30);
			game.bombCounter.SetString(String(format_string, "Mines: %d", mineCount));

			game.restartButtonText.SetFont(&font, 30);
			game.restartButtonText.SetString("Restart");
			game.restartButtonTextColor = Color(255);

			game.restartButton.textureCenterRect = Recti(26, 37, 44, 14);			
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

			if (menu.playButton.GetState() == ButtonState::Down)
				menu.playButtonTextTrans = Math::TranslationMatrix<float>(Vec2i(menu.playButton.colliderRect.pos +
					Vec2i(100 - menu.playButtonText.GetSize().x / 2, (menu.playButtonText.GetSize().y + 10) / 2 - 2)));
			else
				menu.playButtonTextTrans = Math::TranslationMatrix<float>(Vec2i(menu.playButton.colliderRect.pos +
					Vec2i(100 - menu.playButtonText.GetSize().x / 2, (menu.playButtonText.GetSize().y + 10) / 2 + 5)));

			if (menu.playButton.GetState() != ButtonState::Up)
				menu.playButtonTextColor = Color(220);
			else
				menu.playButtonTextColor = Color(255);

			//Render menuTitle
			{ 
				buttonMaterial.properties.mvp = canvasProjection * menu.playButtonTrans;
				buttonMaterial.properties.texture = &buttonTexture;
				Renderer::RenderPointArray(buttonMaterial, menu.playButton.GetMesh());  

				textMaterial.properties.mvp = canvasProjection * menu.playButtonTextTrans;
				textMaterial.properties.texture = menu.playButtonText.GetTexture();
				textMaterial.properties.color = menu.playButtonTextColor.ToVector();
				Renderer::RenderPointArray(textMaterial, menu.playButtonText.GetMesh());

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
			game.bombCounter.SetString(String(format_string, "Mines: %d", mineCount));
			
			if (game.restartButton.GetState() == ButtonState::Pressed)
				ChangeToGameScene();

			if (game.restartButton.GetState() == ButtonState::Down)			
				game.restartButtonTextTrans = Math::TranslationMatrix<float>(Vec2i(game.restartButton.colliderRect.pos +
					Vec2i(100 - game.restartButtonText.GetSize().x / 2, (game.restartButtonText.GetSize().y + 10) / 2 - 2)));			
			else			
				game.restartButtonTextTrans = Math::TranslationMatrix<float>(Vec2i(game.restartButton.colliderRect.pos +
					Vec2i(100 - game.restartButtonText.GetSize().x / 2, (game.restartButtonText.GetSize().y + 10) / 2 + 5)));			

			if (game.restartButton.GetState() != ButtonState::Up)
				game.restartButtonTextColor = Color(220);
			else
				game.restartButtonTextColor = Color(255);

			if (!gameEnded && startedGame)				
				game.timerText.SetString(String(format_string, "Time: %0.2fs.", game.timer.GetTime()));

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

			buttonMaterial.properties.mvp = canvasProjection * game.restartButtonTrans;
			buttonMaterial.properties.texture = &buttonTexture;
			Renderer::RenderPointArray(buttonMaterial, game.restartButton.GetMesh());

			textMaterial.properties.mvp = canvasProjection * game.restartButtonTextTrans;
			textMaterial.properties.texture = game.restartButtonText.GetTexture();
			textMaterial.properties.color = game.restartButtonTextColor.ToVector();
			Renderer::RenderPointArray(textMaterial, game.restartButtonText.GetMesh());

			textMaterial.properties.mvp = canvasProjection * game.timerTextTrans;
			textMaterial.properties.texture = game.timerText.GetTexture();
			textMaterial.properties.color = Color(255).ToVector();
			Renderer::RenderPointArray(textMaterial, game.timerText.GetMesh());

			tilesMaterial.properties.mvp = canvasProjection * tilesTrans;
			tilesMaterial.properties.texture = &tilesSpriteSheet;
			Renderer::RenderPointArray(tilesMaterial, tilesMesh.vl);

			textMaterial.properties.mvp = canvasProjection * game.titleTextTrans;
			textMaterial.properties.texture = game.titleText.GetTexture();
			textMaterial.properties.color = Color(255, 128, 0).ToVector();
			Renderer::RenderPointArray(textMaterial, game.titleText.GetMesh());

			textMaterial.properties.mvp = canvasProjection * game.bombCounterTrans;
			textMaterial.properties.texture = game.bombCounter.GetTexture();
			textMaterial.properties.color = Color(255, 255, 255).ToVector();
			Renderer::RenderPointArray(textMaterial, game.bombCounter.GetMesh());

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
		
		game.detailsText.SetString(String(format_string, "Size is %dx%d", sizeX, sizeY));
		game.timerText.SetString("Time: 0.0");
		game.bombCounter.SetString(String(format_string, "Mines: %d", mineCount));

		memset(valueMatrix, 0, sizeof(valueMatrix));
		memset(checkedMatrix, 0, sizeof(checkedMatrix));
		mineCount = startMineCount;
		gameEnded = false;
		startedGame = false;

		ResizeWindowEvent(window.GetSize().x, window.GetSize().y, &window);
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
		if (!startedGame)
			game.timer.Reset();
		startedGame = true;
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
		app.menu.titleTextTrans = Math::TranslationMatrix<float>(Vec2i(5, h - 5 - app.game.titleText.GetSize().y));
		
		app.menu.playButton.colliderRect = Recti(5, h - 115, 200, 50);
		app.menu.playButton.size = app.menu.playButton.colliderRect.size;
		app.menu.playButtonTrans = Math::TranslationMatrix<float>(app.menu.playButton.colliderRect.pos);
		break;
		}
	case Scene::Game: {
		app.tilesTrans = Math::TranslationMatrix<float>(Vec2i(App::posX, App::posY));
		app.game.titleTextTrans = Math::TranslationMatrix<float>(Vec2i(5, h - 5 - app.game.titleText.GetSize().y));
		
		app.game.restartButton.colliderRect = Recti(5, h - 115, 200, 50);
		app.game.restartButton.size = app.game.restartButton.colliderRect.size;
		app.game.restartButtonTrans = Math::TranslationMatrix<float>(app.game.restartButton.colliderRect.pos);		
		
		app.game.bombCounterTrans = app.game.restartButtonTrans * Math::TranslationMatrix<float>(Vec2i(518, 25 - app.game.timerText.GetSize().y));
		app.game.detailsTextTrans = app.game.restartButtonTrans * Math::TranslationMatrix<float>(Vec2i(210, 45 - app.game.timerText.GetSize().y));
		app.game.timerTextTrans = app.game.restartButtonTrans * Math::TranslationMatrix<float>(Vec2i(210, 25 - app.game.timerText.GetSize().y));
		break;
	}
	}

	app.canvasProjection = Math::OrthographicMatrix<float>(0, w, 0, h, -1, 1);	
	Renderer::SetViewport(Vec2i(), Vec2i(w, h));
}