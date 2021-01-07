#pragma once
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

#include "Terrain.h"

class MainScene : public Scene<MainScene>
{
public:
	Entity cameraEntity;
	Terrain terrain;

	Material terrainMaterial;

	MainScene();
	~MainScene();

	void Load();
	void Save();
	void Unload();

	void Start();
	void Update(float dt);
	void Stop();	
};