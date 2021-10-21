#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Graphics/GraphicsLibrary.h"
#include <vector>

namespace Blaze::UI
{
	class Panel;


	class BLAZE_API PanelRenderer
	{
		Graphics::Core::VertexArray va;
		Graphics::Core::GraphicsBuffer vb;
		Graphics::Core::Program program;
		int uniformVP;

		std::vector<Panel*> panels;
	public:

		PanelRenderer();
		~PanelRenderer();

		void Load();

		void AddPanel(Panel& panel);
		void Render(Mat4f& vp);

		friend class Panel;
	};
}