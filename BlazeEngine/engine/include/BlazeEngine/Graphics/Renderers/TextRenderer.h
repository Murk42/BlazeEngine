#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Graphics/GraphicsLibrary.h"
#include "BlazeEngine/Resources/Font/Font.h"

namespace Blaze
{
	namespace Graphics
	{
		class BLAZE_API TextRenderer
		{
			Core::ShaderProgram program;
			Core::VertexArray va;
			Core::GraphicsBuffer vb;
			Font* font;
		public:
			TextRenderer();
			~TextRenderer();

			void SetProjectionMatrix(Mat4f mat);
			void SetFont(Font* font);

			void Write(const StringViewUTF8& text, Vec2i pos, float height, ColorRGBA color);
		};
	}
}