#pragma once
#include "BlazeEngine/Graphics/GraphicsLibrary.h"
#include "BlazeEngine/Resources/Font/Font.h"
#include "BlazeEngine/Graphics/Renderers/BatchRenderer.h"
#include "BlazeEngine/Application/ECS/Scene.h"
#include <span>

namespace Blaze::UI2
{
	/*
	class BLAZE_API UITextRenderer
	{
		Graphics::Core::GraphicsBuffer vb;		
		void* vbMap;
		uint reservedVBSize;
		uint usedVBSize;
		Graphics::Core::Fence fence;

		Graphics::Core::VertexArray va;
		Graphics::Core::ShaderProgram programNormal;
		Graphics::Core::ShaderProgram programSDF;
		Graphics::Core::ShaderProgram programLCD;
		FontResolution* fontResolution;

		Graphics::Core::ShaderProgram* SelectProgram(FontResolutionRenderType);
		void RenderAll();
	public:
		struct TextRenderData
		{
			StringUTF8 text;
			FontResolution* fontResolution;
			Vec2i pos;
			float fontHeight;
			std::span<ColorRGBAf> colors;
			Rectf clipArea;
		};

		UITextRenderer();

		void ReserveVB(uint size);

		Rectf RenderText(TextRenderData data);
	};
	*/
		

	
}

/*

Frame(400x600, 0x999999ff, 5, 2)
Text("Menu", 30, 0xaaaaaaff)
Frame(400x600, 0x999999ff, 5, 2)
Text("Menu", 30, 0xaaaaaaff)
Frame(400x600, 0x999999ff, 5, 2)
Text("Menu", 30, 0xaaaaaaff)
Frame(400x600, 0x999999ff, 5, 2)

Text("Menu", 30, 0xaaaaaaff)
TextButton(Text("Press me", 30), 200x50, image.png, 0xffffff88, 0.5) //Special (transparent)
Image(image2.png, 20x20)

*/