#include "BlazeEngine/Application/UI System/UIElementTypeRegistry.h"

#include "BlazeEngine/Application/UI System/Core Elements/Button.h"
#include "BlazeEngine/Application/UI System/Core Elements/Image.h"
#include "BlazeEngine/Application/UI System/Core Elements/Panel.h"
#include "BlazeEngine/Application/UI System/Core Elements/Text.h"
#include "BlazeEngine/Application/UI System/Core Elements/TexturedPanel.h"
#include "BlazeEngine/Application/UI System/Core Elements/DropdownSelection.h"
#include "BlazeEngine/Application/UI System/Core Elements/ImageButton.h"
#include "BlazeEngine/Application/UI System/Core Elements/List.h"
#include "BlazeEngine/Application/UI System/Core Elements/SelectionMenu.h"
#include "BlazeEngine/Application/UI System/Core Elements/Slider.h"
#include "BlazeEngine/Application/UI System/Core Elements/TextButton.h"
#include "BlazeEngine/Application/UI System/Core Elements/TextField.h"

namespace Blaze
{
	namespace UI
	{
		Result UIElementTypeRegistry::RegisterType(StringView name, 
			size_t size, void(*construct)(void*), void(*destruct)(void*),
			size_t managerSize, void(*constructManager)(void*), void(*destructManager)(void*))
		{			
			UIElementTypeData data;
			data.name = name;
			data.size = size;
			data.construct = construct;
			data.destruct = destruct;
			data.managerSize = managerSize;
			data.constructManager = constructManager;
			data.destructManager = destructManager;
							
			if (nameTable.try_emplace(name, types.size()).second)
				types.emplace_back(std::move(data));

			return Result();
		}
		Result UIElementTypeRegistry::OverrideManager(StringView name, size_t size, void(*construct)(void*), void(*destruct)(void*))
		{
			auto& data = types[nameTable[name]];
			data.managerSize = size;
			data.constructManager = construct;
			data.destructManager = destruct;
			return Result();
		}
		UIElementTypeRegistry::UIElementTypeRegistry()			
		{
		}
		UIElementTypeRegistry::UIElementTypeRegistry(const UIElementTypeRegistry& other)
			:types(other.types), nameTable(other.nameTable)
		{
		}
		UIElementTypeRegistry::~UIElementTypeRegistry()
		{
			nameTable.clear();
		}
		void UIElementTypeRegistry::RegisterCoreTypes()
		{
			RegisterType<UIElement>();
			RegisterType<Button>();
			RegisterType<Image>();
			RegisterType<Panel>();
			RegisterType<Text>();
			RegisterType<TexturedPanel>();
			RegisterType<UI::List>();
			RegisterType<UI::Slider>();
			RegisterType<UI::TextButton>();
			RegisterType<UI::DropdownSelection>();
			RegisterType<UI::TextField>();
			RegisterType<UI::ImageButton>();
			RegisterType<UI::SelectionMenu>();
		}		
		int UIElementTypeRegistry::GetElementTypeIndex(StringView name) const
		{
			auto it = nameTable.find((String)name);
			if (it == nameTable.end())
				return -1;
			else 
				return it->second;
		}
		uint UIElementTypeRegistry::GetElementTypeCount() const
		{
			return types.size();
		}
		UIElementTypeData UIElementTypeRegistry::GetElementTypeData(uint index) const
		{
			if (index > types.size())
			{				
				Logger::AddLog(LogType::Error, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE,
					"Blaze Engine", "Index is out of bounds. Index was: " + StringParsing::Convert(index).value);					
				return { "" };
			}
			return types[index];
		}
		bool UIElementTypeRegistry::IsValidTypeIndex(uint index) const
		{
			return index < types.size();
		}
		UIElementTypeRegistry UIElementTypeRegistry::CoreRegistry()
		{
			UIElementTypeRegistry registry;
			registry.RegisterCoreTypes();
			return registry;
		}
	}
}