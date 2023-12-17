#pragma once

#include "BlazeEngineCore/Math/Shapes.h"

#include "BlazeEngineCore/Graphics/Renderers/Line2DRenderer.h"
#include "BlazeEngineCore/Graphics/Renderers/Line3DRenderer.h"
#include "BlazeEngineCore/Graphics/Renderers/Point2DRenderer.h"
#include "BlazeEngineCore/Graphics/Renderers/TextRenderer.h"
#include "BlazeEngineCore/Graphics/Renderers/FrameRenderer.h"
#include "BlazeEngineCore/Graphics/Renderers/ImageRenderer.h"
#include "BlazeEngineCore/Graphics/Renderers/Circle2DRenderer.h"
#include "BlazeEngineCore/Graphics/Utility/TextVertexGenerator.h"
#include "BlazeEngineCore/Graphics/Transform.h"

#include "BlazeEngineCore/Event/EventHandler.h"
   
namespace Blaze
{
	namespace Graphics
	{		
		struct CoordinateSystem
		{
			Vec2f origin = Vec2f();
			Vec2f xAxis  = Vec2f(1, 0);
			Vec2f yAxis  = Vec2f(0, 1);
		};
				

		BLAZE_CORE_API Result DrawFrame(Transform2D transform, const FrameGraphicsData& data);
		BLAZE_CORE_API Result DrawFrame(Vec2f pos, Vec2f size, ColorRGBAf fillColor = 0x333333ff, ColorRGBAf borderColor = 0x222222ff, float borderWidth = 2.0f, float cornerRadius = 5.0f);

		BLAZE_CORE_API Result DrawImage(Transform2D transform, const ImageGraphicsData& data);
		BLAZE_CORE_API Result DrawImage(Vec2f pos, Vec2f size, GraphicsLibrary::Texture2D* texture, ColorRGBAf mask = 0xffffffff, float blendFactor = 0.0f);

		BLAZE_CORE_API Result DrawText(Transform2D transform, const TextGraphicsData& data);
		BLAZE_CORE_API Result DrawText(Vec2f pos, Align align, StringViewUTF8 string, float fontHeight, FontResolution* fontResolution = nullptr, ArrayView<ColorRGBAf> colors = { });

		namespace Shapes
		{
			BLAZE_CORE_API Result DrawShape(const Math::Shapes::Line2Df& line, ColorRGBAf color, float width, CoordinateSystem coordinateSystem = CoordinateSystem());
			BLAZE_CORE_API Result DrawShape(const Math::Shapes::Ray2Df& ray, ColorRGBAf color, float width, CoordinateSystem coordinateSystem = CoordinateSystem());
			BLAZE_CORE_API Result DrawShape(const Math::Shapes::Segment2Df& segment, ColorRGBAf color, float width, CoordinateSystem coordinateSystem = CoordinateSystem());
			BLAZE_CORE_API Result DrawShape(const Math::Shapes::Quad2Df& quad, ColorRGBAf color, float width, CoordinateSystem coordinateSystem = CoordinateSystem());

			BLAZE_CORE_API Result DrawPoint2D(Vec2f pos, ColorRGBAf color, float radius);
			BLAZE_CORE_API Result DrawLine2D(Vec2f pos1, Vec2f pos2, ColorRGBAf color, float width);

			BLAZE_CORE_API Result DrawCircle2DOutline(Vec2f center, float radius, ColorRGBAf color, float width);
			BLAZE_CORE_API Result DrawCircle2DFilled(Vec2f center, float radius, ColorRGBAf color);
			BLAZE_CORE_API Result DrawCircle2DFilled(Vec2f center, float outerRadius, float innerRadius, ColorRGBAf outerColor, ColorRGBAf innerColor);

			BLAZE_CORE_API Result DrawBoxOutline2D(Vec2f pos1, Vec2f pos2, ColorRGBAf color, float width);

			BLAZE_CORE_API Result DrawLine3D(Vec3f pos1, Vec3f pos2, ColorRGBAf color, float width);
			BLAZE_CORE_API Result DrawBoxOutline3D(Vec3f pos1, Vec3f pos2, ColorRGBAf color, float width);
		}

		namespace Core
		{
			BLAZE_CORE_API const Point2DRenderer& GetDefaultPoint2DRenderer();
			BLAZE_CORE_API const Line2DRenderer& GetDefaultLine2DRenderer();
			BLAZE_CORE_API const Line3DRenderer& GetDefaultLine3DRenderer();
			BLAZE_CORE_API const FrameRenderer& GetDefaultFrameRenderer();
			BLAZE_CORE_API const TextRenderer& GetDefaultTextRenderer();
			BLAZE_CORE_API const ImageRenderer& GetDefaultTexture2DRenderer();
			BLAZE_CORE_API const Circle2DRenderer& GetDefaultCircle2DRenderer();

			BLAZE_CORE_API Font& GetDefaultFont();

			BLAZE_CORE_API void SetCameraTransform(Vec3f pos, Quatf rot);
			BLAZE_CORE_API void SetCameraTransformMatrix(Mat4f matrix);
			BLAZE_CORE_API void Set3DProjectionMatrix(Mat4f proj);

			BLAZE_CORE_API Mat4f GetCameraProjectionMatrix();
			BLAZE_CORE_API Mat4f GetCameraTransformMatrix();

			BLAZE_CORE_API Vec3f WorldToViewportSpace(Vec3f position);
			BLAZE_CORE_API Vec2f ViewportToScreenSpace(Vec2f position);

		}

		namespace Events
		{
			struct ViewportChangedEvent
			{
				Mat4f matrix;
			};

			BLAZE_CORE_API void AddViewportChangedEventHandler(EventHandler<ViewportChangedEvent>& handler);
			BLAZE_CORE_API void RemoveViewportChangedEventHandler(EventHandler<ViewportChangedEvent>& handler);
		}
	}
}