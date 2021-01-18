#include "MenuScene.h"
#include "App.h"
#include "GameScene.h"

void MenuScene::Startup()
{
	App& app = App::Instance();

	titleText.SetFont(&app.font, 70);
	titleText.SetString("Minesweeper");
	titleText.transform.parent = &app.baseTransform;
	titleText.transform.parentAlign = Align::TopLeft;
	titleText.transform.pos = Vec2f(5, -5);

	playButton.transform.parent = &app.baseTransform;
	playButton.transform.parentAlign = Align::TopLeft;
	playButton.transform.pos = Vec2f(5, -65);
	playButton.transform.size = Vec2f(200, 50);
	playButton.textureCenterRect = app.buttonCenterRect;
	playButtonText.SetFont(&app.font, 30);
	playButtonText.SetString("Play");
	playButtonText.transform.parent = &playButton.transform;
	playButtonText.transform.parentAlign = Align::Center;
	playButtonTextColor = Color(255);
}
void MenuScene::Frame()
{
	App& app = App::Instance();

	playButton.Update();

	if (playButton.GetState() == ButtonState::Down)
		playButtonText.transform.pos = Vec2f(0, 0);
	else
		playButtonText.transform.pos = Vec3f(0, 7);

	if (playButton.GetState() != ButtonState::Up)
		playButtonTextColor = Color(220);
	else
		playButtonTextColor = Color(255);

	playButton.transform.Update();
	playButtonText.transform.Update();
	titleText.transform.Update();

	//Render menuTitle
	{
		app.buttonMaterial.properties.mvp = app.canvasProjection * playButton.transform.mat;
		app.buttonMaterial.properties.texture = &app.buttonTexture;
		Renderer::RenderPointArray(app.buttonMaterial, playButton.GetMesh());

		app.textMaterial.properties.mvp = app.canvasProjection * playButtonText.transform.mat;
		app.textMaterial.properties.texture = playButtonText.GetTexture();
		app.textMaterial.properties.color = playButtonTextColor.ToVector();
		Renderer::RenderPointArray(app.textMaterial, playButtonText.GetMesh());

		app.textMaterial.properties.mvp = app.canvasProjection * titleText.transform.mat;
		app.textMaterial.properties.texture = titleText.GetTexture();
		app.textMaterial.properties.color = Color(255, 128, 0).ToVector();
		Renderer::RenderPointArray(app.textMaterial, titleText.GetMesh());
	}

	if (startingGame && startTimer.GetTime() > 0.2)
		app.ChangeScene<GameScene>();

	if (playButton.GetState() == ButtonState::Down && !startingGame)
	{
		startTimer.Reset();
		startingGame = true;
	}
}
void MenuScene::Cleanup()
{

}