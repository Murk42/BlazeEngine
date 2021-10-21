#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Graphics/Renderer.h"
#include "BlazeEngine/DataStructures/Matrix.h"
#include "BlazeEngine/Resources/Font/Font.h"
#include "BlazeEngine/DataStructures/Transform2D.h"
  
namespace Blaze 
{
	namespace Debug
	{
		void BLAZE_API RenderTexture(Graphics::Core::Texture2D& texture, const Mat4f& mvp);
		void BLAZE_API RenderTexture(Graphics::Core::Texture2D& texture, Vec2i pos, Vec2i size);

		
		void BLAZE_API Write(const String& text, ColorRGBA color, const Mat4f& mvp, Font& font);
		void BLAZE_API Write(const String& text, ColorRGBA color, Vec2f pos, float scale, Align align, Font& font);
		
	}
}