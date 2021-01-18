#pragma once
#include "Scene.h"
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

class MenuScene : public Scene
{
public:
	NormalText titleText;
	Button playButton;
	NormalText playButtonText;
	Color playButtonTextColor;

	Timer startTimer;

	bool startingGame = false;

	void Startup() override;
	void Frame() override;
	void Cleanup() override;
};