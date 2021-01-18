#include "GameScene.h"
#include "App.h"

void GameScene::Startup()
{
	App& app = App::Instance();

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

	tilesSpriteSheet.Load("assets/sprites/SpriteSheet.png", Vec2i(64, 64));
	tilesSpriteSheet.SetSettings(TextureSampling::Linear, TextureSampling::Linear);
	tilesMesh.SetVertices(vertices, maxSizeX * maxSizeY);

	{
		Shader vertexShader = Shader(ShaderType::VertexShader, "assets/shaders/sprite/vertex.glsl");
		Shader fragmentShader = Shader(ShaderType::FragmentShader, "assets/shaders/sprite/fragment.glsl");
		Shader geometryShader = Shader(ShaderType::GeometryShader, "assets/shaders/sprite/geometry.glsl");
		tilesMaterial.SetShaders(vertexShader, fragmentShader, geometryShader);
	}

	tilesTransform.parent = &app.baseTransform;
	tilesTransform.parentAlign = Align::BottomLeft;
	tilesTransform.pos = Vec2f(10, 10);
	tilesTransform.size = Vec2f(sizeX * tileSizeX, sizeY * tileSizeY);

	titleText.SetFont(&app.font, 70);
	titleText.SetString("Minesweeper");
	titleText.transform.parent = &app.baseTransform;
	titleText.transform.parentAlign = Align::TopLeft;
	titleText.transform.pos = Vec2f(5, -5);

	detailsText.SetFont(&app.font, 20);
	detailsText.SetString(String(format_string, "Size is %dx%d", sizeX, sizeY));
	detailsText.transform.parent = &app.baseTransform;
	detailsText.transform.parentAlign = Align::TopLeft;
	detailsText.transform.pos = Vec2f(215, -75);

	restartButton.transform.parent = &app.baseTransform;
	restartButton.transform.parentAlign = Align::TopLeft;
	restartButton.transform.pos = Vec2f(5, -65);
	restartButton.transform.size = Vec2f(200, 50);
	restartButton.textureCenterRect = app.buttonCenterRect;
	restartButtonText.SetFont(&app.font, 30);
	restartButtonText.SetString("Restart");
	restartButtonText.transform.parent = &restartButton.transform;
	restartButtonText.transform.parentAlign = Align::Center;
	restartButtonTextColor = Color(255);

	timerText.SetFont(&app.font, 20);
	timerText.SetString("Time: 0.0");
	timerText.transform.parent = &app.baseTransform;
	timerText.transform.parentAlign = Align::TopLeft;
	timerText.transform.pos = Vec2i(215, -90);
		
	memset(valueMatrix, 0, sizeof(valueMatrix));
	memset(checkedMatrix, 0, sizeof(checkedMatrix));
	mineCount = startMineCount;
	gameEnded = false;
	startedGame = false;

	GenerateTiles();
}
void GameScene::Frame()
{
	App& app = App::Instance();

	restartButton.Update();

	if (restartButton.GetState() == ButtonState::Pressed)
	{
		memset(valueMatrix, 0, sizeof(valueMatrix));
		memset(checkedMatrix, 0, sizeof(checkedMatrix));
		mineCount = startMineCount;
		gameEnded = false;
		startedGame = false;

		GenerateTiles();
	}

	if (restartButton.GetState() == ButtonState::Down)
		restartButtonText.transform.pos = Vec2f(0, 0);
	else
		restartButtonText.transform.pos = Vec2f(0, 7);

	if (restartButton.GetState() != ButtonState::Up)
		restartButtonTextColor = Color(220);
	else
		restartButtonTextColor = Color(255);

	if (!gameEnded && startedGame)
		timerText.SetString(String(format_string, "Time: %0.2fs.", timer.GetTime()));

	if (Input::GetKeyState(Key::MouseLeft) == KeyState::Pressed && !gameEnded)
	{
		Vec2i mp = Input::GetMousePos();
		mp.y = app.window.GetSize().y - mp.y;
		mp -= tilesTransform.pos;
		mp /= Vec2i(tileSizeX, tileSizeY);

		if (mp.x >= 0 && mp.x < sizeX && mp.y >= 0 && mp.y < sizeY)
		{
			int Checker = ClickOnTile(mp.x, mp.y);
			switch (Checker)
			{
			case 0:
				break;
			case 1:
				detailsText.SetString(String("Booooom!"));
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
		mp.y = app.window.GetSize().y - mp.y;
		mp -= tilesTransform.pos;
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

	tilesTransform.Update();
	titleText.transform.Update();
	detailsText.transform.Update();
	restartButton.transform.Update();
	restartButtonText.transform.Update();
	timerText.transform.Update();

	app.buttonMaterial.properties.mvp = app.canvasProjection * restartButton.transform.mat;
	app.buttonMaterial.properties.texture = &app.buttonTexture;
	Renderer::RenderPointArray(app.buttonMaterial, restartButton.GetMesh());

	app.textMaterial.properties.mvp = app.canvasProjection * restartButtonText.transform.mat;
	app.textMaterial.properties.texture = restartButtonText.GetTexture();
	app.textMaterial.properties.color = restartButtonTextColor.ToVector();
	Renderer::RenderPointArray(app.textMaterial, restartButtonText.GetMesh());

	app.textMaterial.properties.mvp = app.canvasProjection * timerText.transform.mat;
	app.textMaterial.properties.texture = timerText.GetTexture();
	app.textMaterial.properties.color = Color(255).ToVector();
	Renderer::RenderPointArray(app.textMaterial, timerText.GetMesh());

	tilesMaterial.properties.mvp = app.canvasProjection * tilesTransform.mat;
	tilesMaterial.properties.texture = &tilesSpriteSheet;
	Renderer::RenderPointArray(tilesMaterial, tilesMesh.vl);

	app.textMaterial.properties.mvp = app.canvasProjection * titleText.transform.mat;
	app.textMaterial.properties.texture = titleText.GetTexture();
	app.textMaterial.properties.color = Color(255, 128, 0).ToVector();
	Renderer::RenderPointArray(app.textMaterial, titleText.GetMesh());

	app.textMaterial.properties.mvp = app.canvasProjection * detailsText.transform.mat;
	app.textMaterial.properties.texture = detailsText.GetTexture();
	app.textMaterial.properties.color = Color(255).ToVector();

	Renderer::RenderPointArray(app.textMaterial, detailsText.GetMesh());
}
void GameScene::Cleanup()
{

}

void GameScene::GenerateTiles()
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

	srand(Time::GetWorldTime() * 1000);
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

void GameScene::UpdateTiles()
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
			detailsText.SetString(String("You won!"));
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

bool GameScene::inRange(int x, int y)
{
	if (x >= 0 && x < sizeX && y >= 0 && y < sizeY)
		return 1;
	else
		return 0;
}

int GameScene::ClickOnTile(int x, int y)
{
	if (!startedGame)
		timer.Reset();
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
