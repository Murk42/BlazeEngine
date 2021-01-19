#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;
 
#include "Scene.h"

struct TextMatProps : MaterialProperties<Mat4f, Texture2D, Vec4f>
{
	Property<Mat4f> mvp = "u_MVP";
	Property<Texture2D> texture = "u_texture";
	Property<Vec4f> color = "u_color";
};
struct ButtonMatProps : MaterialProperties<Mat4f, TextureArray2D, float, Vec4f>
{
	Property<Mat4f> mvp = "u_MVP";
	Property<TextureArray2D> texture = "u_texture";
	Property<float> texScale = "u_texScale";
	Property<Vec4f> textureCenterRect = "u_textureCenterRect";
};

class App : public Application<App>
{
public:
	Console::LogList logList = Console::LogList(Vec2i(0, 0), Vec2i(100, 10));
	Console::Frame frame = Console::Frame(Vec2i(0, 11), Vec2i(100, 10));
	Window window;
	
	Mat4f canvasProjection;

	Scene* activeScene;

	Material<TextMatProps> textMaterial;
	Material<ButtonMatProps> buttonMaterial;

	Font font = Font("assets/fonts/Pixellari.ttf");	
	TextureArray2D buttonTexture = TextureArray2D("assets/sprites/ButtonSpriteSheet.png", Vec2i(96, 96));	

	ButtonData buttonData;

	void Startup() override;
	void Frame() override;
	void Cleanup() override;

	void ProcessLogs();

	template<typename T>
	void ChangeScene()
	{
		if (activeScene != nullptr)
		{
			activeScene->Cleanup();
			delete activeScene;
		}

		activeScene = new T();
		activeScene->Startup();
	}	

	bool inRange(int x, int y);

	int ClickOnTile(int x, int y);
};