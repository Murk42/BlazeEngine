#include "BlazeEngine/Application/UI System/UIScene.h"
#include "BlazeEngine/Core/Result.h"
#include "BlazeEngine/Memory/MemoryManager.h"

#include "BlazeEngine/Application/UI System/Core Elements/Button.h"
#include "BlazeEngine/Application/UI System/Core Elements/Image.h"
#include "BlazeEngine/Application/UI System/Core Elements/Panel.h"
#include "BlazeEngine/Application/UI System/Core Elements/Text.h"
#include "BlazeEngine/Application/UI System/Core Elements/TexturedPanel.h"

namespace Blaze::UI
{
	UIScene::UIScene()
		: manager(nullptr)
	{		
	}
	UIScene::~UIScene()
	{
		if (manager == nullptr)
			return;
		
		while(!elements.empty())
			DestroyElement(elements.back());
	}

	Result UIScene::SetManager(UIManager* manager)
	{
		if (this->manager != nullptr)
			return BLAZE_ERROR_RESULT("BlazeEngine", "You cant change the scene manager");
		else if (manager == nullptr)
			return BLAZE_ERROR_RESULT("BlazeEngine", "You cant set the scene manager to nullptr");

		this->manager = manager;		
		return { };
	}

	void UIScene::SetResourceManager(ResourceSystem::ResourceManager* resourceManager)
	{
		this->resourceStorage.SetResourceManager(resourceManager);
	}	
	
	void UIScene::SetEventFunction(StringView name, const EventFunction& func)
	{		
		functions[name] = func;		
	}	

	ResultValue<UIElement*> UIScene::CreateElement(uint typeIndex, StringView layer)
	{		
		if (manager == nullptr)
			return { nullptr, BLAZE_ERROR_RESULT("BlazeEngine", "Scene manager was not set") };

		if (!manager->HasLayer(layer))
			return { nullptr, BLAZE_WARNING_RESULT("BlazeEngine", "Invalid layer. Layer was: \"" + layer + "\"") };

		const auto& registry = manager->GetElementTypeRegistry();

		if (!registry.IsValidTypeIndex(typeIndex))
			return { nullptr, BLAZE_WARNING_RESULT("BlazeEngine", "Invalid type index. Index was: " + StringParsing::Convert(typeIndex).value) };

		const auto& typeData = registry.GetElementTypeData(typeIndex);		
						
		UIElement*& element = elements.emplace_back();
		element = (UIElement*)Memory::Allocate(typeData.size);
		typeData.construct(element);
		((UIElement*)element)->scene = this;
		((UIElement*)element)->typeIndex = typeIndex;

		if (Result r = manager->AddElement((UIElement*)element, typeIndex, layer))
		{
			typeData.destruct(element);
			Memory::Free(element);
			elements.pop_back();

			return { nullptr, std::move(r) };
		}
		return (UIElement*)element;
	}
	ResultValue<UIElement*> UIScene::CreateElement(StringView name, uint typeIndex, StringView layer)
	{		
		if (manager == nullptr)
			return { nullptr, BLAZE_ERROR_RESULT("BlazeEngine", "Scene manager was not set") };

		auto p = nameToElementMap.insert({ name, nullptr });

		if (p.second)
		{
			auto res = CreateElement(typeIndex, layer);

			if (res.result)
			{
				nameToElementMap.erase(p.first);
				return { nullptr, std::move(res.result) };
			}

			p.first->second = res.value;
			res.value->name = name;

			return res.value;
		}
		else
			return { nullptr, BLAZE_WARNING_RESULT("Blaze Engine", "Trying to create a element, but there already exists a element with the same name") };
	}

	Result UIScene::DestroyElement(UIElement* element)
	{
		if (element->scene != this)
			return Result(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE,
				"BlazeEngine", "Trying to destroy a element from a scene that it doesnt belong to"));

		size_t typeIndex = element->typeIndex;

		if (Result r = manager->RemoveElement(element)) return r;
		
		elements.erase(std::find(elements.begin(), elements.end(), element));
		nameToElementMap.erase(element->name);
		
		manager->GetElementTypeRegistry().GetElementTypeData(typeIndex).destruct(element);
		Memory::Free(element);
		
		return Result();
	}	

	Result UIScene::Load(const Path& path)
	{
		if (path.FileExtension() == ".bin")
		{

		}
		else
		{			
		}

		return Result();
	}		

	//UIElement* UIScene::GetElementBase(StringView name) const
	//{
	//	return nameToElementMap.at(name);
	//}

	UIScene::EventFunction UIScene::GetEventFunction(StringView name) const
	{
		auto it = functions.find(name);
		if (it == functions.end())
			return EventFunction();
		return it->second;
	}
	bool UIScene::IsAvailableName(String name)
	{
		auto it = nameToElementMap.find(name);
		if (it == nameToElementMap.end())
			return true;
		return false;
	}
	UIElement* UIScene::GetElementBase(StringView name) const
	{
		auto it = nameToElementMap.find(name);
		if (nameToElementMap.end() == it)
			return nullptr;
		return 
			it->second;
	}
}