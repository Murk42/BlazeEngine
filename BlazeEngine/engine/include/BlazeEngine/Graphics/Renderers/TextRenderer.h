#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Graphics/GraphicsLibrary.h"

namespace Blaze
{
	class FontResolution;
}

namespace Blaze::Graphics
{	
	class TextRenderCache;
	class BaseTextVertexGenerator;

	struct TextShaderProgramSource
	{
		String vert;
		String geom;
		String frag;
	};	

	BLAZE_API TextShaderProgramSource GetDefaultTextShaderProgramSource_Normal();
	BLAZE_API TextShaderProgramSource GetDefaultTextShaderProgramSource_LCD();
	BLAZE_API TextShaderProgramSource GetDefaultTextShaderProgramSource_SDF();	

	class BLAZE_API TextRenderer
	{
		Core::ShaderProgram program;
		Core::VertexArray va;
		Core::GraphicsBuffer vb;

		FontResolution* fontResolution;
		Mat4f proj;
		
	public:
		TextRenderer();
		~TextRenderer();

		void SetProjectionMatrix(Mat4f mat);
		void SetFontResolution(FontResolution* fontResolution);
		
		Result Write(const StringViewUTF8& text, Vec2f pos, float size, ColorRGBAf color);
		Result Write(const StringViewUTF8& text, Vec2f pos, float size, ColorRGBAf color, BaseTextVertexGenerator& generator);
		Result Write(TextRenderCache&, Vec2f pos, float size, ColorRGBAf color);

		inline Core::ShaderProgram& GetShaderProgram() { return program; };
		inline const Core::ShaderProgram& GetShaderProgram() const { return program; }
		inline FontResolution* GetFontResolution() const { return fontResolution; }
	};
}