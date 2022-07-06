#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Graphics/GraphicsLibrary.h"
#include "BlazeEngine/Resources/Font/Font.h"

namespace Blaze
{
	namespace Graphics
	{
		class TextRenderer;

		class BLAZE_API TextRenderData
		{
			Core::GraphicsBuffer vb;
			Core::VertexArray va;
			unsigned vertexCount;
			Vec2f size;
			Vec2f bottomLeft;
			Vec2f topRight;			
			FontResolution* fontResolution;
			const TextRenderer* renderer;
		public:
			TextRenderData(TextRenderData&&);
			TextRenderData(const TextRenderer& renderer, StringViewUTF8 text, float height);

			Vec2f GetSize() const { return size; }
			Vec2f GetBottomLeft() const { return bottomLeft; }
			Vec2f GetTopRight() const { return topRight; }
			FontResolution* GetFontResolution() const { return fontResolution; }

			friend class TextRenderer;
		};

		class BLAZE_API TextRenderer
		{
			Core::ShaderProgram program;
			Core::VertexArray va;
			Core::GraphicsBuffer vb;
			std::vector<FontResolution*> fontResolutions;			

			FontResolution* SelectResolution(int resolution) const;
		public:
			TextRenderer();
			~TextRenderer();

			void SetProjectionMatrix(Mat4f mat);
			void SetResolutions(std::initializer_list<FontResolution*> resolutions);

			void Write(const StringViewUTF8& text, int resolution, Vec2f pos, ColorRGBA color);
			void Write(TextRenderData&, Vec2f pos, ColorRGBA color);

			friend class TextRenderData;
		};
	}
}