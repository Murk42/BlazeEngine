#include "BlazeEngine/Graphics/Graphics.h"
#include "BlazeEngine/Graphics/Renderer.h"

#include "source/BlazeEngine/Graphics/Shaders.h"
#include "BlazeEngine/Event/Events.h"
#include "BlazeEngine/Math/Math.h"

#include "BlazeEngine/Resources/Font/Font.h"

#include "BlazeEngine/Console/Console.h"

using namespace Blaze::Graphics::Core;
  
namespace Blaze
{
	namespace Graphics
	{
		constexpr size_t maxPoint2DvertexCount = 4096;

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
			TextRenderer textRenderer;
			Font defaultFont;
			FontResolution* fontResolution20;
			FontResolution* fontResolution64;
			FontResolution* fontResolution128;
		};
		static GraphicsData* graphicsData;		

		void Graphics_ViewportChanged(Event::ViewportChanged event)
		{
			graphicsData->proj2D = Math::OrthographicMatrix<float>(0, event.size.x, 0, event.size.y, -1, 1);
			graphicsData->line2DRenderer.SetProjectionMatrix(graphicsData->proj2D);
			graphicsData->point2DRenderer.SetProjectionMatrix(graphicsData->proj2D);
			graphicsData->textRenderer.SetProjectionMatrix(graphicsData->proj2D);

			if (!graphicsData->userProj3D)
			{
				Graphics::Set3DProjectionMatrix(Math::PerspectiveMatrix<float>(90, Renderer::GetViewportRatio(), 0.1, 1000));
				graphicsData->line3DRenderer.SetProjectionMatrix(graphicsData->VP3D);
			}
		}
	}
	
	void InitializeGraphics()
	{
		Graphics::graphicsData = new Graphics::GraphicsData;		

		Graphics::graphicsData->line2DRenderer.SetImmediateMode();
		Graphics::graphicsData->line3DRenderer.SetImmediateMode();
		Graphics::graphicsData->point2DRenderer.SetImmediateMode();		

		Graphics::graphicsData->userProj3D = false;
		Graphics::graphicsData->view3D = Mat4f::Identity();
		Graphics::graphicsData->line3DRenderer.SetProjectionMatrix(Graphics::graphicsData->proj3D);

		Graphics::graphicsData->defaultFont.Load("assets/Blaze/Consola.ttf");
		Graphics::graphicsData->fontResolution20 = Graphics::graphicsData->defaultFont.CreateFontResolution(20);
		Graphics::graphicsData->fontResolution64 = Graphics::graphicsData->defaultFont.CreateFontResolution(64);
		Graphics::graphicsData->fontResolution128 = Graphics::graphicsData->defaultFont.CreateFontResolution(128);		

		Graphics::graphicsData->textRenderer.SetResolutions({
			Graphics::graphicsData->fontResolution20,
			Graphics::graphicsData->fontResolution64,
			Graphics::graphicsData->fontResolution128
			});
	}
	void TerminateGraphics()
	{
		delete Graphics::graphicsData;
	}

	namespace Graphics
	{
		void DrawPoint2D(Vec2f pos, ColorRGBA color, float radius)
		{
			graphicsData->point2DRenderer.Draw(pos, color, radius);
		}
		void DrawLine2D(Vec2f pos1, Vec2f pos2, ColorRGBA color, float width)
		{	
			graphicsData->line2DRenderer.Draw(pos1, pos2, color, width);
		}
		void DrawBoxOutline2D(Vec2f pos1, Vec2f pos2, ColorRGBA color, float width)
		{
			Graphics::DrawLine2D(Vec2f(pos1.x, pos1.y), Vec2f(pos1.x, pos2.y), color, width);
			Graphics::DrawLine2D(Vec2f(pos1.x, pos2.y), Vec2f(pos2.x, pos2.y), color, width);
			Graphics::DrawLine2D(Vec2f(pos2.x, pos2.y), Vec2f(pos2.x, pos1.y), color, width);
			Graphics::DrawLine2D(Vec2f(pos2.x, pos1.y), Vec2f(pos1.x, pos1.y), color, width);
		}
		void DrawLine3D(Vec3f pos1, Vec3f pos2, ColorRGBA color, float width)
		{
			graphicsData->line3DRenderer.Draw(pos1, pos2, color, width);
		}
		void DrawBoxOutline3D(Vec3f pos1, Vec3f pos2, ColorRGBA color, float width)
		{
			Graphics::DrawLine3D(Vec3f(pos1.x, pos1.y, pos1.z), Vec3f(pos2.x, pos1.y, pos1.z), color, width);
			Graphics::DrawLine3D(Vec3f(pos1.x, pos2.y, pos1.z), Vec3f(pos2.x, pos2.y, pos1.z), color, width);
			Graphics::DrawLine3D(Vec3f(pos1.x, pos1.y, pos2.z), Vec3f(pos2.x, pos1.y, pos2.z), color, width);
			Graphics::DrawLine3D(Vec3f(pos1.x, pos2.y, pos2.z), Vec3f(pos2.x, pos2.y, pos2.z), color, width);
			Graphics::DrawLine3D(Vec3f(pos1.x, pos1.y, pos1.z), Vec3f(pos1.x, pos2.y, pos1.z), color, width);
			Graphics::DrawLine3D(Vec3f(pos2.x, pos1.y, pos1.z), Vec3f(pos2.x, pos2.y, pos1.z), color, width);
			Graphics::DrawLine3D(Vec3f(pos1.x, pos1.y, pos2.z), Vec3f(pos1.x, pos2.y, pos2.z), color, width);
			Graphics::DrawLine3D(Vec3f(pos2.x, pos1.y, pos2.z), Vec3f(pos2.x, pos2.y, pos2.z), color, width);
			Graphics::DrawLine3D(Vec3f(pos1.x, pos1.y, pos1.z), Vec3f(pos1.x, pos1.y, pos2.z), color, width);
			Graphics::DrawLine3D(Vec3f(pos2.x, pos1.y, pos1.z), Vec3f(pos2.x, pos1.y, pos2.z), color, width);
			Graphics::DrawLine3D(Vec3f(pos1.x, pos2.y, pos1.z), Vec3f(pos1.x, pos2.y, pos2.z), color, width);
			Graphics::DrawLine3D(Vec3f(pos2.x, pos2.y, pos1.z), Vec3f(pos2.x, pos2.y, pos2.z), color, width);
		}
		void Write(const StringViewUTF8& text, float resolution, Vec2f pos, ColorRGBA color)
		{ 
			graphicsData->textRenderer.Write(text, resolution, pos, color);
		}
		void Write(TextRenderData& data, Vec2f pos, ColorRGBA color)
		{
			graphicsData->textRenderer.Write(data, pos, color);
		}

		TextRenderData GetTextRenderData(const StringViewUTF8& text, float height)
		{
			return TextRenderData(graphicsData->textRenderer, text, height);
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
			return graphicsData->textRenderer;
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
		void Set3DProjectionMatrix(const Mat4f& mat)
		{
			graphicsData->userProj3D = true;
			graphicsData->proj3D = mat;
			graphicsData->VP3D = graphicsData->proj3D * graphicsData->view3D;
			graphicsData->line3DRenderer.SetProjectionMatrix(graphicsData->VP3D);
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
			transformed.x /= transformed.w;
			transformed.y /= transformed.w;
			Vec3f out = Vec3f((transformed.xy + Vec2f(1.0f)) / 2.0f * (Vec2f)Renderer::GetViewportSize(), transformed.w);
			return out;
		}
	}	
}