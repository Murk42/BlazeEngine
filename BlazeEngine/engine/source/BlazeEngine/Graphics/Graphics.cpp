#include "BlazeEngine/Graphics/Graphics.h"
#include "BlazeEngine/Graphics/GraphicsCore.h"

#include "source/BlazeEngine/Graphics/Shaders.h"
#include "BlazeEngine/Event/Events.h"
#include "BlazeEngine/Math/Math.h"

#include "BlazeEngine/Resources/Font/Font.h"

#include "BlazeEngine/Console/Console.h"

#include "BlazeEngine/Core/Startup.h"

using namespace Blaze::Graphics::Core;
  
namespace Blaze
{
	namespace Graphics
	{		
		struct GraphicsData
		{
			bool userProj3D = false;

			Mat4f proj3D;
			Mat4f view3D;
			Mat4f VP3D;
			Mat4f proj2D;

			Line2DRenderer line2DRenderer;
			Line3DRenderer line3DRenderer;
			Point2DRenderer point2DRenderer;
			//TextRenderer textRenderer;
			//FrameRenderer frameRenderer;

			ShaderProgram drawTexShaderProgram;
			VertexArray drawTexVA;
			ImmutableDynamicGraphicsBuffer drawTexVB;

			Font defaultFont;			
		};
		static GraphicsData* graphicsData = nullptr;		

		void Graphics_ViewportChanged(Event::ViewportChanged event)
		{
			graphicsData->proj2D = Math::OrthographicMatrix<float>(0, event.size.x, 0, event.size.y, -1, 1);
			graphicsData->line2DRenderer.SetProjectionMatrix(graphicsData->proj2D);
			graphicsData->point2DRenderer.SetProjectionMatrix(graphicsData->proj2D);
			//graphicsData->textRenderer.SetProjectionMatrix(graphicsData->proj2D);

			if (!graphicsData->userProj3D)
			{
				Graphics::Set3DProjectionMatrix(Math::PerspectiveMatrix<float>(90, Graphics::Core::GetViewportRatio(), 0.1f, 1000.0f));
				graphicsData->line3DRenderer.SetProjectionMatrix(graphicsData->VP3D);
			}
		}
	}

	void InitializeCoreGraphics();
	void TerminateCoreGraphics();
	
	Startup::GraphicsInitInfo InitializeGraphics()
	{
		using namespace Graphics;

		Startup::GraphicsInitInfo initInfo;
		TimePoint startTimePoint = TimePoint::GetWorldTime();

		InitializeCoreGraphics();
		
		graphicsData = new GraphicsData();

		graphicsData->line2DRenderer.SetBatchMode(4);
		graphicsData->line3DRenderer.SetBatchMode(12);
		graphicsData->point2DRenderer.SetBatchMode(1);

		graphicsData->userProj3D = false;
		graphicsData->view3D = Mat4f::Identity();		

		graphicsData->defaultFont.Load("assets/default/fonts/Consola.ttf");		
		{
			FontResolution* fontRes = graphicsData->defaultFont.CreateFontResolution(8, FontResolutionRenderType::HorizontalLCD);
			fontRes->LoadCharacters(0, 127);
			fontRes->CreateAtlas();
		}
		{
			FontResolution* fontRes = graphicsData->defaultFont.CreateFontResolution(12, FontResolutionRenderType::Antialiased);
			fontRes->LoadCharacters(0, 127);
			fontRes->CreateAtlas();
		}
		{
			FontResolution* fontRes = graphicsData->defaultFont.CreateFontResolution(16, FontResolutionRenderType::Antialiased);
			fontRes->LoadCharacters(0, 127);
			fontRes->CreateAtlas();
		}
		{
			FontResolution* fontRes = graphicsData->defaultFont.CreateFontResolution(24, FontResolutionRenderType::Antialiased);
			fontRes->LoadCharacters(0, 127);
			fontRes->CreateAtlas();
		}
		{
			FontResolution* fontRes = graphicsData->defaultFont.CreateFontResolution(32, FontResolutionRenderType::SDF);
			fontRes->LoadCharacters(0, 127);
			fontRes->CreateAtlas();
		}

		{
			const char* vSource =
				"#version 450 core						  \n"
				"										  \n"
				"layout(location = 0) in vec3 i_pos;	  \n"
				"layout(location = 1) in vec2 i_uv;		  \n"
				"										  \n"
				"layout(location = 0) uniform mat4 u_MVP; \n"
				"										  \n"
				"out vec2 p_uv;							  \n"
				"										  \n"
				"void main()							  \n"
				"{										  \n"
				"	p_uv = i_uv;						  \n"
				"	gl_Position = u_MVP * vec4(i_pos, 1); \n"
				"}										  \n";
			const char* fSource =
				"#version 450 core									\n"
				"													\n"
				"in vec2 p_uv;										\n"
				"													\n"
				"layout(location = 1) uniform sampler2D u_texture;	\n"
				"													\n"
				"void main()										\n"
				"{													\n"
				"	vec4 color = texture(u_texture, p_uv);			\n"
				"													\n"
				"	gl_FragColor = color;							\n"
				"}													\n";
			VertexShader vs; vs.ShaderSource(vSource); vs.CompileShader();
			FragmentShader fs; fs.ShaderSource(fSource); fs.CompileShader();
			graphicsData->drawTexShaderProgram.LinkShaders({ &vs, &fs });
			
			graphicsData->drawTexVA.EnableVertexAttribute(0);
			graphicsData->drawTexVA.EnableVertexAttribute(1);
			graphicsData->drawTexVA.SetVertexAttributeFormat(0, VertexAttributeType::Float, 3, false, 0);
			graphicsData->drawTexVA.SetVertexAttributeFormat(1, VertexAttributeType::Float, 2, false, 3 * sizeof(float));
			graphicsData->drawTexVA.SetVertexAttributeBuffer(0, &graphicsData->drawTexVB, 5 * sizeof(float), 0);
			graphicsData->drawTexVA.SetVertexAttributeBuffer(1, &graphicsData->drawTexVB, 5 * sizeof(float), 0);

			graphicsData->drawTexVB.Allocate(nullptr, sizeof(float) * 5);
		}

		initInfo.initTime = TimePoint::GetWorldTime() - startTimePoint;
		return initInfo;
	}
	void TerminateGraphics()
	{
		delete Graphics::graphicsData;		

		TerminateCoreGraphics();
	}

	namespace Graphics
	{
		void DrawPoint2D(Vec2f pos, ColorRGBAf color, float radius)
		{
			auto& r = graphicsData->point2DRenderer;
			r.Draw(pos, color, radius);
			r.Flush();
		}
		void DrawLine2D(Vec2f pos1, Vec2f pos2, ColorRGBAf color, float width)
		{	
			auto& r = graphicsData->line2DRenderer;			
			r.Draw(pos1, pos2, color, width);
			r.Flush();
		}
		void DrawBoxOutline2D(Vec2f pos1, Vec2f pos2, ColorRGBAf color, float width)
		{
			auto& r = graphicsData->line2DRenderer;			
			r.Draw(Vec2f(pos1.x, pos1.y), Vec2f(pos1.x, pos2.y), color, width);
			r.Draw(Vec2f(pos1.x, pos2.y), Vec2f(pos2.x, pos2.y), color, width);
			r.Draw(Vec2f(pos2.x, pos2.y), Vec2f(pos2.x, pos1.y), color, width);
			r.Draw(Vec2f(pos2.x, pos1.y), Vec2f(pos1.x, pos1.y), color, width);
			r.Flush();
		}
		void DrawLine3D(Vec3f pos1, Vec3f pos2, ColorRGBAf color, float width)
		{
			auto& r = graphicsData->line3DRenderer;			
			r.Draw(pos1, pos2, color, width);
			r.Flush();
		}
		void DrawBoxOutline3D(Vec3f pos1, Vec3f pos2, ColorRGBAf color, float width)
		{
			auto& r = graphicsData->line3DRenderer;			
			r.Draw(Vec3f(pos1.x, pos1.y, pos1.z), Vec3f(pos2.x, pos1.y, pos1.z), color, width);
			r.Draw(Vec3f(pos1.x, pos2.y, pos1.z), Vec3f(pos2.x, pos2.y, pos1.z), color, width);
			r.Draw(Vec3f(pos1.x, pos1.y, pos2.z), Vec3f(pos2.x, pos1.y, pos2.z), color, width);
			r.Draw(Vec3f(pos1.x, pos2.y, pos2.z), Vec3f(pos2.x, pos2.y, pos2.z), color, width);
			r.Draw(Vec3f(pos1.x, pos1.y, pos1.z), Vec3f(pos1.x, pos2.y, pos1.z), color, width);
			r.Draw(Vec3f(pos2.x, pos1.y, pos1.z), Vec3f(pos2.x, pos2.y, pos1.z), color, width);
			r.Draw(Vec3f(pos1.x, pos1.y, pos2.z), Vec3f(pos1.x, pos2.y, pos2.z), color, width);
			r.Draw(Vec3f(pos2.x, pos1.y, pos2.z), Vec3f(pos2.x, pos2.y, pos2.z), color, width);
			r.Draw(Vec3f(pos1.x, pos1.y, pos1.z), Vec3f(pos1.x, pos1.y, pos2.z), color, width);
			r.Draw(Vec3f(pos2.x, pos1.y, pos1.z), Vec3f(pos2.x, pos1.y, pos2.z), color, width);
			r.Draw(Vec3f(pos1.x, pos2.y, pos1.z), Vec3f(pos1.x, pos2.y, pos2.z), color, width);
			r.Draw(Vec3f(pos2.x, pos2.y, pos1.z), Vec3f(pos2.x, pos2.y, pos2.z), color, width);
			r.Flush();
		}		
		Result DrawFrame(Vec2f pos, Vec2f size, ColorRGBAf fillColor, ColorRGBAf borderColor, float borderWidth, float cornerRadius)
		{
			//graphicsData->frameRenderer.SetProjectionMatrix(graphicsData->proj2D);
			//CHECK_RESULT(graphicsData->frameRenderer.Render(pos, size, fillColor, borderColor, borderWidth, cornerRadius));			
			return Result();
		}
		void DrawTexture(Vec2f p1, Vec2f p2, Graphics::Core::Texture2D& tex, Vec2f uv1, Vec2f uv2)
		{
			struct Vertex {
				Vec3f p;
				Vec2f uv;
			} vertices[]{
				{ { p1.x, p1.y, 0.0f }, { uv1.x, uv1.y } },
				{ { p1.x, p2.y, 0.0f }, { uv1.x, uv2.y } },
				{ { p2.x, p1.y, 0.0f }, { uv2.x, uv1.y } },
				{ { p2.x, p2.y, 0.0f }, { uv2.x, uv2.y } },
			};
			graphicsData->drawTexVB.WriteData(vertices, sizeof(vertices), 0);
			Graphics::Core::SelectVertexArray(&graphicsData->drawTexVA);
			Graphics::Core::SelectProgram(&graphicsData->drawTexShaderProgram);
			graphicsData->drawTexShaderProgram.SetUniform(0, graphicsData->proj2D);
			graphicsData->drawTexShaderProgram.SetUniform(1, 0);
			Graphics::Core::SetActiveTextureSlot(0);
			Graphics::Core::SelectTexture(&tex);

			Graphics::Core::RenderPrimitiveArray(Graphics::Core::PrimitiveType::TriangleStrip, 0, 4);			
		}
		void DrawTexture(Vec2f p1, Vec2f p2, Graphics::Core::Texture1D& tex, float uv1, float uv2)
		{
			return void();
		}
		Result Write(Vec2f pos, StringViewUTF8 text, float fontHeight, std::span<const ColorRGBAf> colors)
		{			
			FontResolution* fontResolution = graphicsData->defaultFont.GetClosestResolution(fontHeight);
			return ADD_STACK_FRAME(Write(pos, text, fontHeight, colors, fontResolution));
		}
		Result Write(Vec2f pos, StringViewUTF8 text, float fontHeight, std::initializer_list<ColorRGBAf> colors)
		{
			auto span = std::span<const ColorRGBAf>(colors.begin(), colors.end());
			return ADD_STACK_FRAME(Write(pos, text, fontHeight, span));
		}
		Result Write(Vec2f pos, StringViewUTF8 text, float fontHeight, std::span<const ColorRGBAf> colors, FontResolution* fontResolution)
		{
			//graphicsData->textRenderer.SetProjectionMatrix(graphicsData->proj2D);
			//return ADD_STACK_FRAME(graphicsData->textRenderer.Write(pos, text, fontHeight, fontResolution, colors));
			return Result();
		}		

		Point2DRenderer& GetPoint2DRenderer()
		{
			return graphicsData->point2DRenderer;
		}
		Line2DRenderer& GetLine2DRenderer()
		{
			return graphicsData->line2DRenderer;
		}
		Line3DRenderer& GetLine3DRenderer()
		{
			return graphicsData->line3DRenderer;
		}
		TextRenderer& GetTextRenderer()
		{
			return *(TextRenderer*)nullptr;
			//return graphicsData->textRenderer;
		}

		Font& GetDefaultFont()
		{
			return graphicsData->defaultFont;
		}

		void Set3DViewMatrix(const Mat4f& mat)
		{
			graphicsData->view3D = mat;
			graphicsData->VP3D = graphicsData->proj3D * graphicsData->view3D;
			graphicsData->line3DRenderer.SetProjectionMatrix(graphicsData->VP3D);
		}		
		Mat4f Set3DViewMatrix(Vec3f pos, Quatf rot)
		{
			Mat4f mat = Math::RotationMatrix(rot.Conjugated()) * Math::TranslationMatrix(-pos);
			Set3DViewMatrix(mat);
			return mat;
		}
		void Set3DProjectionMatrix(const Mat4f& mat)
		{
			graphicsData->userProj3D = true;
			graphicsData->proj3D = mat;
			graphicsData->VP3D = graphicsData->proj3D * graphicsData->view3D;
			graphicsData->line3DRenderer.SetProjectionMatrix(graphicsData->VP3D);
		}	
		Mat4f Set3DProjectionMatrix(float fov, float near, float far)
		{
			Mat4f mat = Math::PerspectiveMatrix(fov, Graphics::Core::GetViewportRatio(), near, far);
			Set3DProjectionMatrix(mat);
			return mat;
		}
		Mat4f Get3DViewMatrix()
		{			
			return graphicsData->view3D;
		}
		Mat4f Get3DProjectionMatrix()
		{			
			return graphicsData->proj3D;
		}

		Vec3f WorldToScreenSpace(Vec3f position)
		{
			//is in range [-1, 1]
			Vec4f transformed = graphicsData->proj3D * graphicsData->view3D * Vec4f(position, 1.0f);			
			transformed.xyz /= transformed.w;
			Vec3f out = Vec3f((transformed.xy + Vec2f(1.0f)) / 2.0f * (Vec2f)Graphics::Core::GetViewportSize(), transformed.w);
			return out;
		}		
	}	
}