#pragma once
#include "BlazeEngineGraphics/UI/Core/UINode.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderStreams/OpenGL/ImageRenderStream_OpenGL.h"

//TODO remove need for OpenGL specific textures
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper//Textures/OpenGLTexture2D.h"

namespace Blaze::UI
{
	class Image : public Node
	{
	public:
		Graphics::OpenGLWrapper::Texture2D* texture;
		Vec2f uv1, uv2;
		ColorRGBAf color;
		float blend, alpha;

		Image(Node* parent, const NodeTransform& transform);

		String GetTypeName() const { return "Image"; }
	private:
		Graphics::OpenGL::ImageRenderStream_OpenGL renderStream;
	};
}