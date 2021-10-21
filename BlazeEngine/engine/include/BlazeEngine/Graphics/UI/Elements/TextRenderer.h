#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Graphics/GraphicsLibrary.h"

#include <unordered_map>

namespace Blaze
{
	class Font;
}

namespace Blaze::UI
{
	class Text;

	struct TextVertex
	{
		Vec2f p1;
		Vec2f p2;
		Vec2f uv1;
		Vec2f uv2;
		Vec4f color;
	};
	class BLAZE_API TextRenderer
	{
		Graphics::Core::Program program;
		Graphics::Core::GraphicsBuffer vb;
		Graphics::Core::VertexArray va;
		int uniformTex;
		int uniformVP;
		
		std::vector<Text*> texts;		
	public:
		TextRenderer();

		void Load();
		void AddText(Text&);
		void Render(const Mat4f& VP);

		friend class Text;
	};
}