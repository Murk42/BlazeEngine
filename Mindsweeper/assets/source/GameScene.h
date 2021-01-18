#pragma once
#include "Scene.h"
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

class GameScene : public Scene
{
public:	
	Mesh tilesMesh;
	Transform2D tilesTransform;

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

	Button restartButton;
	NormalText restartButtonText;
	Color restartButtonTextColor;
	NormalText titleText;
	NormalText detailsText;
	NormalText timerText;
	Timer timer;

	void Startup() override;
	void Frame() override;
	void Cleanup() override;

	void GenerateTiles();
	void UpdateTiles();
	bool inRange(int x, int y);
	int ClickOnTile(int x, int y);	
};