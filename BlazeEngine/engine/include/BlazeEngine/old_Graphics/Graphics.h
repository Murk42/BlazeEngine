#pragma once

#include "BlazeEngine/Math/Shapes.h"

#include "BlazeEngine/Graphics/Renderers/Line2DRenderer.h"
#include "BlazeEngine/Graphics/Renderers/Line3DRenderer.h"
#include "BlazeEngine/Graphics/Renderers/Point2DRenderer.h"
#include "BlazeEngine/Graphics/Renderers/TextRenderer.h"
#include "BlazeEngine/Graphics/Renderers/FrameRenderer.h"
#include "BlazeEngine/Graphics/Renderers/ImageRenderer.h"
#include "BlazeEngine/Graphics/Renderers/Circle2DRenderer.h"
#include "BlazeEngine/Graphics/Utility/TextVertexGenerator.h"
#include "BlazeEngine/Graphics/Transform.h"

#include "BlazeEngine/Event/EventHandler.h"
   
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
				

		BLAZE_API Result DrawFrame(Transform2D transform, const FrameGraphicsData& data);
		BLAZE_API Result DrawFrame(Vec2f pos, Vec2f size, ColorRGBAf fillColor = 0x333333ff, ColorRGBAf borderColor = 0x222222ff, float borderWidth = 2.0f, float cornerRadius = 5.0f);

		BLAZE_API Result DrawImage(Transform2D transform, const ImageGraphicsData& data);
		BLAZE_API Result DrawImage(Vec2f pos, Vec2f size, GraphicsLibrary::Texture2D* texture, ColorRGBAf mask = 0xffffffff, float blendFactor = 0.0f);

		BLAZE_API Result DrawText(Transform2D transform, const TextGraphicsData& data);
		BLAZE_API Result DrawText(Vec2f pos, Align align, StringViewUTF8 string, float fontHeight, FontResolution* fontResolution = nullptr, ArrayView<ColorRGBAf> colors = { });

		namespace Shapes
		{
			BLAZE_API Result DrawShape(const Math::Shapes::Line2Df& line, ColorRGBAf color, float width, CoordinateSystem coordinateSystem = CoordinateSystem());
			BLAZE_API Result DrawShape(const Math::Shapes::Ray2Df& ray, ColorRGBAf color, float width, CoordinateSystem coordinateSystem = CoordinateSystem());
			BLAZE_API Result DrawShape(const Math::Shapes::Segment2Df& segment, ColorRGBAf color, float width, CoordinateSystem coordinateSystem = CoordinateSystem());
			BLAZE_API Result DrawShape(const Math::Shapes::Quad2Df& quad, ColorRGBAf color, float width, CoordinateSystem coordinateSystem = CoordinateSystem());

			BLAZE_API Result DrawPoint2D(Vec2f pos, ColorRGBAf color, float radius);
			BLAZE_API Result DrawLine2D(Vec2f pos1, Vec2f pos2, ColorRGBAf color, float width);

			BLAZE_API Result DrawCircle2DOutline(Vec2f center, float radius, ColorRGBAf color, float width);
			BLAZE_API Result DrawCircle2DFilled(Vec2f center, float radius, ColorRGBAf color);
			BLAZE_API Result DrawCircle2DFilled(Vec2f center, float outerRadius, float innerRadius, ColorRGBAf outerColor, ColorRGBAf innerColor);

			BLAZE_API Result DrawBoxOutline2D(Vec2f pos1, Vec2f pos2, ColorRGBAf color, float width);

			BLAZE_API Result DrawLine3D(Vec3f pos1, Vec3f pos2, ColorRGBAf color, float width);
			BLAZE_API Result DrawBoxOutline3D(Vec3f pos1, Vec3f pos2, ColorRGBAf color, float width);
		}

		namespace Core
		{
			BLAZE_API const Point2DRenderer& GetDefaultPoint2DRenderer();
			BLAZE_API const Line2DRenderer& GetDefaultLine2DRenderer();
			BLAZE_API const Line3DRenderer& GetDefaultLine3DRenderer();
			BLAZE_API const FrameRenderer& GetDefaultFrameRenderer();
			BLAZE_API const TextRenderer& GetDefaultTextRenderer();
			BLAZE_API const ImageRenderer& GetDefaultTexture2DRenderer();
			BLAZE_API const Circle2DRenderer& GetDefaultCircle2DRenderer();

			BLAZE_API Font& GetDefaultFont();

			BLAZE_API void SetCameraTransform(Vec3f pos, Quatf rot);
			BLAZE_API void SetCameraTransformMatrix(Mat4f matrix);
			BLAZE_API void Set3DProjectionMatrix(Mat4f proj);

			BLAZE_API Mat4f GetCameraProjectionMatrix();
			BLAZE_API Mat4f GetCameraTransformMatrix();

			BLAZE_API Vec3f WorldToViewportSpace(Vec3f position);
			BLAZE_API Vec2f ViewportToScreenSpace(Vec2f position);

		}

		namespace Events
		{
			struct ViewportChangedEvent
			{
				Mat4f matrix;
			};

			BLAZE_API void AddViewportChangedEventHandler(EventHandler<ViewportChangedEvent>& handler);
			BLAZE_API void RemoveViewportChangedEventHandler(EventHandler<ViewportChangedEvent>& handler);
		}
	}
}