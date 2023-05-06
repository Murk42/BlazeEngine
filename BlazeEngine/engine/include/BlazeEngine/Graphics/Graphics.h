#pragma once
#include "BlazeEngine/DataStructures/Color.h" 
#include "BlazeEngine/DataStructures/Quaternion.h"
#include "BlazeEngine/DataStructures/Matrix.h"
#include "BlazeEngine/DataStructures/StringViewUTF8.h"
#include "BlazeEngine/Graphics/Renderers/Line2DRenderer.h"
#include "BlazeEngine/Graphics/Renderers/Line3DRenderer.h"
#include "BlazeEngine/Graphics/Renderers/Point2DRenderer.h"
#include "BlazeEngine/Graphics/Renderers/TextRenderer.h"
#include "BlazeEngine/Graphics/Renderers/FrameRenderer.h"
   
namespace Blaze 
{   
	class Font;

	namespace Graphics
	{

		BLAZE_API void DrawPoint2D(Vec2f pos, ColorRGBAf color, float radius);		
		BLAZE_API void DrawLine2D(Vec2f pos1, Vec2f pos2, ColorRGBAf color, float width);
		//BLAZE_API void DrawCircleOutline(Vec2f pos, float radius, ColorRGBA color, float width);
		//BLAZE_API void DrawCircle(Vec2f pos, float radius, ColorRGBA color, float width);
		BLAZE_API void DrawBoxOutline2D(Vec2f pos1, Vec2f pos2, ColorRGBAf color, float width);
		//BLAZE_API void DrawBox2D(Vec2f pos1, Vec2f pos2, ColorRGBA color, float width);

		//BLAZE_API void DrawPoint3D(Vec3f pos, ColorRGBA color, float radius);
		BLAZE_API void DrawLine3D(Vec3f pos1, Vec3f pos2, ColorRGBAf color, float width);
		BLAZE_API void DrawBoxOutline3D(Vec3f pos1, Vec3f pos2, ColorRGBAf color, float width);
		//BLAZE_API void DrawBox3D(Vec3f pos1, Vec3f pos2, ColorRGBA color, float width);

		BLAZE_API Result DrawFrame(Vec2f pos, Vec2f size, ColorRGBAf fillColor = 0x222222ff, ColorRGBAf borderColor = 0x333333ff, float borderWidth = 2, float cornerRadius = 8);
		
		BLAZE_API void DrawTexture(Vec2f p1, Vec2f p2, Graphics::Core::Texture2D& tex, Vec2f uv1 = Vec2f(0), Vec2f uv2 = Vec2f(1));
		BLAZE_API void DrawTexture(Vec2f p1, Vec2f p2, Graphics::Core::Texture1D& tex, float uv1 = 0, float uv2 = 1);
		
		BLAZE_API Result Write(Vec2f pos, StringViewUTF8 text, float fontHeight, std::span<const ColorRGBAf> colors);	
		BLAZE_API Result Write(Vec2f pos, StringViewUTF8 text, float fontHeight, std::initializer_list<ColorRGBAf> colors);
		BLAZE_API Result Write(Vec2f pos, StringViewUTF8 text, float fontHeight, std::span<const ColorRGBAf> colors, FontResolution* fontResolution);		
		BLAZE_API Result Write(Vec2f pos, StringViewUTF8 text, float fontHeight, std::initializer_list<ColorRGBAf> colors, FontResolution* fontResolution);

		BLAZE_API Point2DRenderer& GetPoint2DRenderer();
		BLAZE_API Line2DRenderer& GetLine2DRenderer();
		BLAZE_API Line3DRenderer& GetLine3DRenderer();
		BLAZE_API TextRenderer& GetTextRenderer();

		BLAZE_API Font& GetDefaultFont();				

		BLAZE_API void Set3DViewMatrix(const Mat4f&);		
		BLAZE_API Mat4f Set3DViewMatrix(Vec3f pos, Quatf rot);
		BLAZE_API void Set3DProjectionMatrix(const Mat4f&);		
		BLAZE_API Mat4f Set3DProjectionMatrix(float fov, float near, float far);
		BLAZE_API Mat4f Get3DViewMatrix();
		BLAZE_API Mat4f Get3DProjectionMatrix();

		BLAZE_API Vec3f WorldToScreenSpace(Vec3f position);		
	}
}