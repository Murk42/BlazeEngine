#pragma once
#include "BlazeEngineGraphics/Renderers/OpenGL/TexturedRectRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Core/RenderStream.h"

namespace Blaze::UI
{
	class Text;
}

namespace Blaze::Graphics::OpenGL
{
	class AtlasData_OpenGL
	{
	public:
		struct UVRect
		{
			Vec2f uv1;
			Vec2f uv2;
		};

		UVRect GetUVRect(UnicodeChar character) const;
		inline OpenGLWrapper::Texture2D* GetAtlas() { return &atlas; }
	private:
		OpenGLWrapper::Texture2D atlas;

		Map<UnicodeChar, UVRect> uvs;

		friend class FontAtlasesData_OpenGL;
	};

	class FontAtlasesData_OpenGL
	{	
	public:
		FontAtlasesData_OpenGL(Font* font, Array<uint> fontHeights, const CharacterSet& characterSet);

		AtlasData_OpenGL* GetAtlasData(uint fontHeight);
	private:
		struct AtlasData
		{
			AtlasData_OpenGL atlasData;
			uint fontHeight;
		};
		Array<AtlasData> atlasData;
	};

	class TextRenderStream_OpenGL :
		public RenderStream
	{
		UI::Text* text;
		
		Font* font;
		uint fontHeight;
		uint characterIndex;
		Vec2f pos;
		Vec2f up;
		Vec2f right;
		float scale;
		Rectf cullingRect;

		AtlasData_OpenGL* atlasData;
		TexturedRectRenderData_OpenGL rd;
	public:
		TextRenderStream_OpenGL(UI::Text* text);

		void BeginStream() override;
		void* GetCurrent() override;
		void Advance() override;
		bool IsEmpty() override;
	};
}