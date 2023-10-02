#include "BlazeEngine/Graphics/Graphics.h"
#include "BlazeEngine/Graphics/GraphicsCore.h"
#include "BlazeEngine/Core/Window.h"

#include "BlazeEngine/Graphics/Shaders.h"
#include "BlazeEngine/Graphics/Utility/TextVertexGenerator.h"

#include "BlazeEngine/Resources/Font/Font.h"

#include "BlazeEngine/File/FileSystem.h"
#include "BlazeEngine/Console/Console.h"

using namespace Blaze::Graphics::Core;
  
namespace Blaze
{
	namespace Graphics
	{			
		static Mat4f viewportMatrix;
		static Mat4f proj3D;
		static Mat4f view3D;
		static Mat4f VP3D;			
		static Math::Shapes::Quad2Df viewportQuad;
		static Vec2f viewportSize;		

		static EventDispatcher<Events::ViewportChangedEvent> viewportChangedDispatcher;

		struct GraphicsResources
		{			

			Line2DRenderer line2DRenderer;
			Line3DRenderer line3DRenderer;
			Point2DRenderer point2DRenderer;
			TextRenderer textRenderer;
			FrameRenderer frameRenderer;			
			ImageRenderer imageRenderer;
			Circle2DRenderer circle2DRenderer;

			Font defaultFont;			
		};
		static GraphicsResources* resources = nullptr;				

		class RenderAreaChangedHandler : public EventHandler<Renderer::Events::RenderAreaChanged>
		{
			void OnEvent(Renderer::Events::RenderAreaChanged event) override
			{
				viewportSize = (Vec2f)event.size;
				viewportMatrix = Mat4f::OrthographicMatrix(0, viewportSize.x, 0, viewportSize.y, -1, 1);
				proj3D = Mat4f::PerspectiveMatrix(Math::PI / 2, viewportSize.x / viewportSize.y, 0.1, 10000);

				resources->line2DRenderer.SetProjectionMatrix(viewportMatrix);
				resources->point2DRenderer.SetProjectionMatrix(viewportMatrix);
				resources->textRenderer.SetProjectionMatrix(viewportMatrix);
				resources->frameRenderer.SetProjectionMatrix(viewportMatrix);
				resources->imageRenderer.SetProjectionMatrix(viewportMatrix);
				resources->circle2DRenderer.SetProjectionMatrix(viewportMatrix);

			}
		} renderAreaChangedHandler;		
	}

	TimingResult InitializeCoreGraphics();
	void TerminateCoreGraphics();

	FontResolution* LoadFontResolution(const Path& path, uint resolution, FontResolutionRenderType renderType, CharacterSet set)
	{
		FontResolution* fr = nullptr;
		using namespace Graphics;
		if (FileSystem::Exists(path).value)
			fr = resources->defaultFont.LoadFontResolution(path);

		if (fr == nullptr)
		{
			fr = resources->defaultFont.CreateFontResolution(resolution, renderType, set);
			fr->Save(path);			
		}

		return fr;
	}

	TimingResult LoadGraphicsResources()
	{
		using namespace Graphics;

		Timing timing{ "Graphics resources" };

		resources = new GraphicsResources();
		
		{
			Timing timing2{ "Default font loading" };
			resources->defaultFont.Load("assets/default/fonts/Consola.ttf");
			timing.AddNode(timing2);
		}

		{
			Timing timing2{ "Creation font resolutions" };
			
			FontResolution* fontRes08 = LoadFontResolution("assets\\cache\\Consola08.bl_fa", 8, FontResolutionRenderType::HorizontalLCD, { { { 0, 127 } } });
			FontResolution* fontRes12 = LoadFontResolution("assets\\cache\\Consola12.bl_fa", 12, FontResolutionRenderType::Antialiased, { { { 0, 127 } } });
			FontResolution* fontRes16 = LoadFontResolution("assets\\cache\\Consola16.bl_fa", 16, FontResolutionRenderType::Antialiased, { { { 0, 127 } } });
			FontResolution* fontRes24 = LoadFontResolution("assets\\cache\\Consola24.bl_fa", 24, FontResolutionRenderType::Antialiased, { { { 0, 127 } } });
			FontResolution* fontRes36 = LoadFontResolution("assets\\cache\\Consola36.bl_fa", 32, FontResolutionRenderType::SDF, { { { 0, 127 } } });			

			timing.AddNode(timing2);
		}

		return timing.GetTimingResult();
	}
	
	TimingResult InitializeGraphics()
	{		
		using namespace Graphics;

		Timing timing{ "Graphics" };		
		
		Graphics::Renderer::Events::AddRenderAreaChangedEventHandler(renderAreaChangedHandler);

		timing.AddNode(InitializeCoreGraphics());
		timing.AddNode(LoadGraphicsResources());

		return timing.GetTimingResult();
	}
	void TerminateGraphics()
	{
		delete Graphics::resources;

		TerminateCoreGraphics();
	}

	namespace Graphics
	{				
		Result DrawFrame(Transform2D transform, const FrameGraphicsData& data)
		{
			return resources->frameRenderer.Render(transform, data);
		}

		Result DrawFrame(Vec2f pos, Vec2f size, ColorRGBAf fillColor, ColorRGBAf borderColor, float borderWidth, float cornerRadius)
		{
			Transform2D transform{ .pos = pos };
			FrameGraphicsData data{ .size = size, .fillColor = fillColor, .borderColor = borderColor, .borderWidth = borderWidth, .cornerRadius = cornerRadius };
			return resources->frameRenderer.Render(transform, data);
		}

		Result DrawImage(Transform2D transform, const ImageGraphicsData& data)
		{
			return resources->imageRenderer.Render(transform, data);
		}

		Result DrawImage(Vec2f pos, Vec2f size, GraphicsLibrary::Texture2D* texture, ColorRGBAf mask, float blendFactor)
		{
			Transform2D transform{ .pos = pos };
			ImageGraphicsData data{ .size = size, .texture = texture, .uvs = { Vec2f(0, 0), Vec2f(0, 1), Vec2f(1, 0), Vec2f(1, 1) }, .mask = mask, .blendFactor = blendFactor};
			return resources->imageRenderer.Render(transform, data);
		}

		Result DrawText(Transform2D transform, const TextGraphicsData& data)
		{
			return resources->textRenderer.Render(transform, data);
		}
		  
		Result DrawText(Vec2f pos, Align align, StringViewUTF8 string, float fontHeight, FontResolution* fontResolution, ArrayView<ColorRGBAf> colors)
		{
			Transform2D transform{ .pos = pos };

			switch (align)
			{
			case Blaze::Align::Top:	        transform.pivot = Vec2f(0.5,   0); break;
			case Blaze::Align::TopRight:    transform.pivot = Vec2f(  1,   0); break;
			case Blaze::Align::Right:		transform.pivot = Vec2f(  1,-0.5); break;		
			case Blaze::Align::BottomRight:	transform.pivot = Vec2f(  1,  -1); break;			
			case Blaze::Align::Bottom:		transform.pivot = Vec2f(0.5,  -1); break;		
			case Blaze::Align::BottomLeft:	transform.pivot = Vec2f(  0,  -1); break;			
			case Blaze::Align::Left:		transform.pivot = Vec2f(  0,-0.5); break;		
			case Blaze::Align::TopLeft:		transform.pivot = Vec2f(  0,   0); break;		
			case Blaze::Align::Center:		transform.pivot = Vec2f(0.5,-0.5); break;	
			}

			if (fontResolution == nullptr)
				fontResolution = resources->defaultFont.GetClosestResolution(fontHeight);

			DefaultTextVertexGenerator vertexGenerator;
			TextGraphicsData data;
			CHECK_RESULT(data.GenerateVertices(string, fontHeight, fontResolution, colors, vertexGenerator));

			return resources->textRenderer.Render(transform, data);
		}

		namespace Core
		{
			const Point2DRenderer& GetDefaultPoint2DRenderer()
			{
				return resources->point2DRenderer;
			}
			const Line2DRenderer& GetDefaultLine2DRenderer()
			{
				return resources->line2DRenderer;
			}
			const Line3DRenderer& GetDefaultLine3DRenderer()
			{
				return resources->line3DRenderer;
			}
			const FrameRenderer& GetDefaultFrameRenderer()
			{
				return resources->frameRenderer;
			}
			const TextRenderer& GetDefaultTextRenderer()
			{
				return resources->textRenderer;
			}
			const ImageRenderer& GetDefaultTexture2DRenderer()
			{
				return resources->imageRenderer;
			}
			const Circle2DRenderer& GetDefaultCircle2DRenderer()
			{
				return resources->circle2DRenderer;
			}

			Font& GetDefaultFont()
			{
				return resources->defaultFont;
			}

			void SetCameraTransform(Vec3f pos, Quatf rot)
			{
				view3D = Mat4f::RotationMatrix(rot.Conjugated()) * Mat4f::TranslationMatrix(-pos);
			}

			void SetCameraTransformMatrix(Mat4f matrix)
			{
				view3D = matrix;
			}

			void Set3DProjectionMatrix(Mat4f proj)
			{
				proj3D = proj;
			}

			Mat4f GetCameraProjectionMatrix()
			{
				return proj3D;
			}

			Mat4f GetCameraTransformMatrix()
			{
				return view3D;
			}

			Vec3f WorldToViewportSpace(Vec3f position)
			{
				//is in range [-1, 1]
				Vec4f transformed = proj3D * view3D * Vec4f(position, 1.0f);
				transformed.x /= transformed.w;
				transformed.y /= transformed.w;
				transformed.z /= transformed.w;
				Vec3f out = Vec3f((transformed.xy() + Vec2f(1.0f)) / 2.0f * viewportSize, transformed.w);
				return out;
			}
			Vec2f ViewportToScreenSpace(Vec2f position)
			{
				return Vec2f();
			}
		}

		namespace Shapes
		{
			bool IntersectViewport(const Math::Shapes::Line2Df& line, Vec2f& p1, Vec2f& p2)
			{
				return Math::Shapes::Intersection(viewportQuad, line, p1, p2);
			}
			bool IntersectViewport(const Math::Shapes::Ray2Df& ray, Vec2f& p1, Vec2f& p2)
			{
				std::array<Vec2f, 2> points;
				uint count = 0;

				if (Math::Shapes::Intersection(viewportQuad, ray, points, count))
				{
					p1 = points[0];
					p2 = count == 1 ? ray.point : points[0];

					return true;
				}

				return false;
			}
			bool IntersectViewport(const Math::Shapes::Segment2Df& segment, Vec2f& p1, Vec2f& p2)
			{
				std::array<Vec2f, 2> points;
				uint count = 0;

				if (Math::Shapes::Intersection(viewportQuad, segment, points, count))
				{
					p1 = points[0];
					p2 = count == 1 ? (viewportQuad.IsInside(segment.pointA) ? segment.pointA : segment.pointB) : points[1];

					return true;
				}

				if (viewportQuad.IsInside(segment.pointA))
				{
					p1 = segment.pointA;
					p2 = segment.pointB;

					return true;
				}

				return false;
			}

			Result DrawShape(const Math::Shapes::Line2Df& line, ColorRGBAf color, float width, CoordinateSystem coordinateSystem)
			{
				auto& r = resources->line2DRenderer;

				Math::Shapes::Line2Df transformedLine =
					line.ApplyAffineTransformation(Mat2f({
						coordinateSystem.xAxis.x, coordinateSystem.yAxis.x,
						coordinateSystem.xAxis.y, coordinateSystem.yAxis.y
						}), coordinateSystem.origin);

				Vec2f pos1, pos2;

				if (IntersectViewport(transformedLine, pos1, pos2))
					return r.Draw(pos1, pos2, color, width);

				return Result();
			}
			Result DrawShape(const Math::Shapes::Ray2Df& ray, ColorRGBAf color, float width, CoordinateSystem coordinateSystem)
			{
				auto& r = resources->line2DRenderer;

				Math::Shapes::Ray2Df transformedRay =
					ray.ApplyAffineTransformation(Mat2f({
						coordinateSystem.xAxis.x, coordinateSystem.yAxis.x,
						coordinateSystem.xAxis.y, coordinateSystem.yAxis.y
						}), coordinateSystem.origin);

				Vec2f pos1, pos2;

				if (IntersectViewport(transformedRay, pos1, pos2))
					return r.Draw(pos1, pos2, color, width);

				return Result();
			}
			Result DrawShape(const Math::Shapes::Segment2Df& segment, ColorRGBAf color, float width, CoordinateSystem coordinateSystem)
			{
				auto& r = resources->line2DRenderer;

				Math::Shapes::Segment2Df transformedSegment =
					segment.ApplyAffineTransformation(Mat2f({
						coordinateSystem.xAxis.x, coordinateSystem.yAxis.x,
						coordinateSystem.xAxis.y, coordinateSystem.yAxis.y
						}), coordinateSystem.origin);

				Vec2f pos1, pos2;

				if (IntersectViewport(transformedSegment, pos1, pos2))
					CHECK_RESULT(r.Draw(pos1, pos2, color, width));

				return Result();
			}
			Result DrawShape(const Math::Shapes::Quad2Df& quad, ColorRGBAf color, float width, CoordinateSystem coordinateSystem)
			{
				auto& r = resources->line2DRenderer;

				Math::Shapes::Quad2Df transformedQuad =
					quad.ApplyAffineTransformation(Mat2f({
						coordinateSystem.xAxis.x, coordinateSystem.yAxis.x,
						coordinateSystem.xAxis.y, coordinateSystem.yAxis.y
						}), coordinateSystem.origin);

				for (uint i = 0; i < 4; ++i)
				{
					Math::Shapes::Segment2Df segment{ transformedQuad.points[i], transformedQuad.points[(i + 1) & 3] };
					Vec2f pos1, pos2;

					if (IntersectViewport(segment, pos1, pos2))
						CHECK_RESULT(r.Draw(pos1, pos2, color, width));
				}

				return Result();
			}
			Result DrawPoint2D(Vec2f pos, ColorRGBAf color, float radius)
			{
				auto& r = resources->point2DRenderer;
				r.Draw(pos, color, radius);

				return Result();
			}
			Result DrawLine2D(Vec2f pos1, Vec2f pos2, ColorRGBAf color, float width)
			{
				auto& r = resources->line2DRenderer;
				r.Draw(pos1, pos2, color, width);

				return Result();
			}

			Result DrawCircle2DOutline(Vec2f center, float radius, ColorRGBAf color, float width)
			{
				auto& r = resources->circle2DRenderer;
				CHECK_RESULT(r.Draw(center, radius + width / 2, radius - width / 2, color, ColorRGBAf(color.r, color.g, color.b, 0)));

				return Result();
			}
			Result DrawCircle2DFilled(Vec2f center, float radius, ColorRGBAf color)
			{
				auto& r = resources->circle2DRenderer;
				CHECK_RESULT(r.Draw(center, radius, 0, color, 0));

				return Result();
			}
			Result DrawCircle2DFilled(Vec2f center, float outerRadius, float innerRadius, ColorRGBAf outerColor, ColorRGBAf innerColor)
			{
				auto& r = resources->circle2DRenderer;
				CHECK_RESULT(r.Draw(center, outerRadius, innerRadius, outerColor, innerColor));

				return Result();
			}

			Result DrawBoxOutline2D(Vec2f pos1, Vec2f pos2, ColorRGBAf color, float width)
			{
				auto& r = resources->line2DRenderer;
				r.Draw(Vec2f(pos1.x, pos1.y), Vec2f(pos1.x, pos2.y), color, width);
				r.Draw(Vec2f(pos1.x, pos2.y), Vec2f(pos2.x, pos2.y), color, width);
				r.Draw(Vec2f(pos2.x, pos2.y), Vec2f(pos2.x, pos1.y), color, width);
				r.Draw(Vec2f(pos2.x, pos1.y), Vec2f(pos1.x, pos1.y), color, width);

				return Result();
			}
			Result DrawLine3D(Vec3f pos1, Vec3f pos2, ColorRGBAf color, float width)
			{
				auto& r = resources->line3DRenderer;
				CHECK_RESULT(r.Draw(pos1, pos2, color, width));
				return Result();
			}
			Result DrawBoxOutline3D(Vec3f pos1, Vec3f pos2, ColorRGBAf color, float width)
			{
				auto& r = resources->line3DRenderer;
				CHECK_RESULT(r.Draw(Vec3f(pos1.x, pos1.y, pos1.z), Vec3f(pos2.x, pos1.y, pos1.z), color, width));
				CHECK_RESULT(r.Draw(Vec3f(pos1.x, pos2.y, pos1.z), Vec3f(pos2.x, pos2.y, pos1.z), color, width));
				CHECK_RESULT(r.Draw(Vec3f(pos1.x, pos1.y, pos2.z), Vec3f(pos2.x, pos1.y, pos2.z), color, width));
				CHECK_RESULT(r.Draw(Vec3f(pos1.x, pos2.y, pos2.z), Vec3f(pos2.x, pos2.y, pos2.z), color, width));
				CHECK_RESULT(r.Draw(Vec3f(pos1.x, pos1.y, pos1.z), Vec3f(pos1.x, pos2.y, pos1.z), color, width));
				CHECK_RESULT(r.Draw(Vec3f(pos2.x, pos1.y, pos1.z), Vec3f(pos2.x, pos2.y, pos1.z), color, width));
				CHECK_RESULT(r.Draw(Vec3f(pos1.x, pos1.y, pos2.z), Vec3f(pos1.x, pos2.y, pos2.z), color, width));
				CHECK_RESULT(r.Draw(Vec3f(pos2.x, pos1.y, pos2.z), Vec3f(pos2.x, pos2.y, pos2.z), color, width));
				CHECK_RESULT(r.Draw(Vec3f(pos1.x, pos1.y, pos1.z), Vec3f(pos1.x, pos1.y, pos2.z), color, width));
				CHECK_RESULT(r.Draw(Vec3f(pos2.x, pos1.y, pos1.z), Vec3f(pos2.x, pos1.y, pos2.z), color, width));
				CHECK_RESULT(r.Draw(Vec3f(pos1.x, pos2.y, pos1.z), Vec3f(pos1.x, pos2.y, pos2.z), color, width));
				CHECK_RESULT(r.Draw(Vec3f(pos2.x, pos2.y, pos1.z), Vec3f(pos2.x, pos2.y, pos2.z), color, width));
				return Result();
			}
		}

		namespace Events
		{
			void AddViewportChangedEventHandler(EventHandler<ViewportChangedEvent>& handler)
			{
				viewportChangedDispatcher.AddHandler(handler);
			}
			void RemoveViewportChangedEventHandler(EventHandler<ViewportChangedEvent>& handler)
			{
				viewportChangedDispatcher.RemoveHandler(handler);
			}
		}
	}	
}

