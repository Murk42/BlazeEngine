#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

class App : Application<App>
{
public:	
	static constexpr Vec2i gridPos = Vec2i(2, 2);
	static constexpr Vec2i gridSize = Vec2i(10, 10);
	static constexpr int gridTileSize = 32;
	static constexpr int gridTileOffset = 2;

	Window window;	
	Mat4f canvasProjMat;	

	struct {

		Graphics::VertexArray va;
		Graphics::ImmutableBuffer<true, false> vb;		
		Graphics::Program program;

		int MVPuniform;
		Mat4f MVP;
	} grid;	

	void Startup() override
	{		
		window.ShowWindow(true);		
		Renderer::SetTarget(window);
		Renderer::SetViewport(Vec2i(), window.GetSize());

		Input::SetEventFunction<InputEvent::WindowClosed>(WindowClosedEvent);
		Input::SetEventFunction<InputEvent::WindowSizeChanged>(WindowSizeChangedEvent);

		//Setting up grid
		{			
			struct Vertex {
				Vec4f rect;
				Vec4f color;
			} vertices[gridSize.x * gridSize.y];			

			int i = 0;
			Vec2i offset = gridPos;
			for (int y = 0; y < gridSize.y; ++y)
			{
				for (int x = 0; x < gridSize.x; ++x)
				{
					vertices[i].rect = Vec4f(offset.x, offset.y, gridTileSize, gridTileSize);
					vertices[i].color = Vec4f(0.2, 0.2, 0.2, 1);
					++i;					
					offset.x += gridTileSize + gridTileOffset;
				}
				offset.x = gridPos.x;
				offset.y += gridTileSize + gridTileOffset;
			}
			

			grid.vb.AllocateData(sizeof(vertices), vertices);			
			
			grid.va.SetVertexAttributeData(0, Graphics::AttributeData(&grid.vb, sizeof(Vertex), 0, Graphics::AttributeType::Vec4f, false, 0));
			grid.va.SetVertexAttributeData(1, Graphics::AttributeData(&grid.vb, sizeof(Vertex), 0, Graphics::AttributeType::Vec4f, false, sizeof(Vec4f)));						

			Graphics::Shader vertexShader = Graphics::Shader(Graphics::ShaderType::VertexShader, "assets/vertex.glsl");
			Graphics::Shader geometryShader = Graphics::Shader(Graphics::ShaderType::GeometryShader, "assets/geometry.glsl");
			Graphics::Shader fragmentShader = Graphics::Shader(Graphics::ShaderType::FragmentShader, "assets/fragment.glsl");

			if (grid.program.LinkShaders({ &vertexShader, &geometryShader, &fragmentShader }) == BLAZE_ERROR)
			{
				Logger::AddLog(LogType::Error, "A", grid.program.GetLinkingLog());
			}

			grid.MVPuniform = grid.program.GetUniformLocation("u_MVP");			
		}				
	} 
	  
	void Cleanup() override 
	{ 

	} 
	
	void Frame() override
	{
		Input::Update();
		Time::Update();
		Render(); 

		if (Input::GetKeyState(Key::T).state == KeyState::Pressed)
		{
			Console::Write("Bound VertexArray:\n");
			Graphics::VertexArray* va = Graphics::VertexArray::GetBound();
			if (va != nullptr)
			{
				Console::Write(String(format_string, "Adress: %p\n", va));								
			}
			else
				Console::Write("\nnone");
		}
	}

	void Render()
	{
		Renderer::ClearTarget();

		//Render grid
		{
			grid.MVP = canvasProjMat;
			grid.program.SetUniform(grid.MVPuniform, grid.MVP);

			grid.program.Use();
			Graphics::VertexArray::Bind(grid.va);
			Renderer::RenderPointArray(gridSize.x * gridSize.y, 0);
		}

		Renderer::UpdateTarget();
	}

	void NewLog(const Log& log) override
	{
		Console::Write(log.source + ": " + log.message + '\n');
	}
			
	static void WindowClosedEvent(Window& win)
	{
		Stop();
	}
	static void WindowSizeChangedEvent(int x, int y, Window& win)
	{
		auto& app = App::Instance();
		Renderer::SetViewport({}, { x, y });
		app.canvasProjMat = Math::OrthographicMatrix(0, x, 0, y, -1, 1);
	}
};