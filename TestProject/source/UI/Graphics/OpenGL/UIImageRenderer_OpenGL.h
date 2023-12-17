#pragma once
#include "BlazeEngineGraphics/OpenGLWrapper/Textures/OpenGLTexture2D.h"

namespace UI2
{
	struct ImageRenderData_OpenGL
	{

	};

	struct ImageRenderCache_OpenGL
	{
		struct Vertex
		{
			Vec2f pos;
			Vec2f uv;			
			Vec4f color;
			float blend;
		};

		Array<Graphics::OpenGLWrapper::Texture2D*> textures;
		Array<Vertex> vertices;
	};

	class ImageRenderer_OpenGL
	{
	public:
		void CreateCache(ArrayView<ImageRenderData_OpenGL> renderData, ImageRenderCache_OpenGL& renderCache);
		void Render(const ImageRenderCache_OpenGL& renderCache, Vec2u targetSize);
	private:
	};
}