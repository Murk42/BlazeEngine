#pragma once
#include "BlazeEngine/Graphics/GraphicsLibrary.h"
#include "BlazeEngine/Graphics/Renderers/BatchStreamRenderer.h"
#include "BlazeEngine/Resources/Font/Font.h"
#include <span>

namespace Blaze
{
	class FontResolution;
}

namespace Blaze::Graphics
{	
	class TextRenderCache;
	class BaseTextVertexGenerator;

	class BLAZE_API TextRenderer
	{
	public:
		enum class TextRenderingMethod
		{
			Normal,
			SDF,
			LCD
		};
		TextRenderer();
		~TextRenderer();

		void SetProjectionMatrix(Mat4f mat);		
		
		Result Write(Vec2f pos, StringViewUTF8 text, float fontHeight, FontResolution* fontResolution, std::span<const ColorRGBAf> colors);
		Result Write(Vec2f pos, StringViewUTF8 text, float fontHeight, FontResolution* fontResolution, std::span<const ColorRGBAf> colors, BaseTextVertexGenerator& generator);
		Result Write(Vec2f pos, TextRenderCache&);		

		Result SetShaders(std::initializer_list<Graphics::Core::Shader*> normalShaders, TextRenderingMethod methodType);
		Result LoadDefaultShaders(TextRenderingMethod methodType);
		Result LoadDefaultShaders();
	private:
		Graphics::Core::ShaderProgram programNormal;
		Graphics::Core::ShaderProgram programSDF;
		Graphics::Core::ShaderProgram programLCD;
		Graphics::Core::VertexArray va;
		Mat4f proj;		

		//BatchStreamRenderer renderer;

		bool GetProgram(TextRenderingMethod method, Graphics::Core::ShaderProgram*& program);
	};
}