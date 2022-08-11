#include "BlazeEngine/Application/UI System/Core Elements/TexturedPanel.h"

namespace Blaze
{ 
	namespace UI
	{
		TexturedPanel::TexturedPanel()
			: shown(true), mask(0, 0, 0, 0),
			sourceRect(0, 0, 100, 100), texture(nullptr), dimensions({ 20, 20, 20, 20 })
		{
		}

		void TexturedPanelManager::Setup()
		{
		}
		void TexturedPanelManager::Render(size_t index, size_t end)
		{
		}
	}
}