#pragma once
#include "BlazeEngineGraphics/UI/Core/UINode.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderStreams/OpenGL/PanelRenderStream_OpenGL.h"

namespace Blaze::UI
{
	class Panel : public Node
	{
	public:
		ColorRGBAf fillColor, borderColor;
		float cornerRadius, borderWidth;

		Panel(Node* parent, const NodeTransform& transform);

		String GetTypeName() const { return "Panel"; }
	private:
		Graphics::OpenGL::PanelRenderStream_OpenGL renderStream;
	};
}