#include "BlazeEngine/Application/UI System/UIManager.h"
#include "BlazeEngine/Input/Input.h"
#include "BlazeEngine/Graphics/Renderer.h"
#include "BlazeEngine/Math/Math.h"
#include "BlazeEngine/Core/MemoryManager.h"
#include "BlazeEngine/Math/Random.h"
#include "BlazeEngine/Application/UI System/UIElement.h"

#include "BlazeEngine/Application/UI System/Core Elements/Button.h"

namespace Blaze::UI
{ 
	Rectf RectIntersection(Rectf a, Rectf b)
	{			
		Vec2f a1 = a.pos;
		Vec2f a2 = a1 + a.size;

		Vec2f b1 = b.pos;
		Vec2f b2 = b1 + b.size;

		a1.x = std::clamp(b1.x, a1.x, a2.x);
		a1.y = std::clamp(b1.y, a1.y, a2.y);
		a2.x = std::clamp(b2.x, a1.x, a2.x);
		a2.y = std::clamp(b2.y, a1.y, a2.y);

		return { a1, a2 - a1 };
	}

	void UIManager::ResolveClipRect(UIElement* element)
	{
		if (element->clipElement != nullptr)
			element->clipRect = { element->clipElement->GetViewportPos(), element->clipElement->GetSize() };
		else
			element->clipRect = { Vec2f(), (Vec2f)viewport.size };

		element->clipRect = RectIntersection(element->clipRect, { Vec2f(), (Vec2f)viewport.size });
	}
	void UIManager::ResolveAlignedRect(UIElement* element)
	{
		Vec2f size;
		Vec2f offset;

		if (element->anchor != nullptr)
		{
			size = element->anchor->GetSize();
			offset = element->anchor->GetViewportPos();
		}
		else
		{
			size = (Vec2f)viewport.size;
			offset = Vec2f();
		}

		switch (element->anchorAlignment)
		{
		case Blaze::Align::Top:			offset += Vec2f(size.x / 2, size.y); break;
		case Blaze::Align::TopRight:	offset += Vec2f(size.x, size.y); break;
		case Blaze::Align::Right:		offset += Vec2f(size.x, size.y / 2); break;
		case Blaze::Align::BottomRight:	offset += Vec2f(size.x, 0); break;
		case Blaze::Align::Bottom:		offset += Vec2f(size.x / 2, 0); break;
		case Blaze::Align::BottomLeft:	offset += Vec2f(0, 0); break;
		case Blaze::Align::Left:		offset += Vec2f(0, size.y / 2); break;
		case Blaze::Align::TopLeft:		offset += Vec2f(0, size.y); break;
		case Blaze::Align::Center:		offset += Vec2f(size.x / 2, size.y / 2); break;
		}

		size = element->size;

		switch (element->localAlignment)
		{
		case Blaze::Align::Top:			offset -= Vec2f(size.x / 2, size.y); break;
		case Blaze::Align::TopRight:	offset -= Vec2f(size.x, size.y); break;
		case Blaze::Align::Right:		offset -= Vec2f(size.x, size.y / 2); break;
		case Blaze::Align::BottomRight:	offset -= Vec2f(size.x, 0); break;
		case Blaze::Align::Bottom:		offset -= Vec2f(size.x / 2, 0); break;
		case Blaze::Align::BottomLeft:	offset -= Vec2f(0, 0); break;
		case Blaze::Align::Left:		offset -= Vec2f(0, size.y / 2); break;
		case Blaze::Align::TopLeft:		offset -= Vec2f(0, size.y); break;
		case Blaze::Align::Center:		offset -= Vec2f(size.x / 2, size.y / 2); break;
		}

		element->viewportPos = element->pos + offset;
		element->viewportPos.x = std::round(element->viewportPos.x);
		element->viewportPos.y = std::round(element->viewportPos.y);
	}
	void UIManager::CleanElement(UIElement* element)
	{
		if (element->updateState == updateState)
			return;		
			
		if (element->anchor != nullptr)
		{
			CleanElement(element->anchor);
			element->flags.set(1, element->anchor->IsActive());
		}
		else
			element->flags.set(1, true);

		if (element->clipElement != nullptr)
			CleanElement(element->clipElement);

		ResolveClipRect(element);
		ResolveAlignedRect(element);

		element->updateState = updateState;

		for (auto tied : element->tiedElements)
			CleanElement(tied);
	}

	Result UIManager::SetupElementManagers()
	{
		for (uint i = 0; i < typeCount; ++i)
			if (managers[i] != nullptr)
				managers[i]->Setup();

		return Result();
	}
	Result UIManager::CleanupElementManagers()
	{
		for (uint i = 0; i < typeCount; ++i)
			if (managers[i] != nullptr)
				managers[i]->Cleanup();

		return Result();
	}

	UIManager::UIManager()
		: managers(nullptr), typeCount(0), updateState(0), focusedElement(nullptr), blockingElement(nullptr)
	{
		SetViewport(Vec2i(), Renderer::GetViewportSize());
	}
	UIManager::~UIManager()
	{
		for (auto it = layers.rbegin(); it != layers.rend(); ++it)
		{
			while (!it->elements.empty())
			{
				auto element = it->elements.back();

				if (element->GetScene() != nullptr)
					Logger::AddLog(BLAZE_FATAL_LOG("BlazeEngine", "Destructing a UI manager while there exist some scenes that reference it"));

				RemoveElement(element);
			}
		}

		CleanupElementManagers();

		for (int i = 0; i < typeCount; ++i)
		{
			const auto& typeData = this->registry.GetElementTypeData(i);

			typeData.destructManager(managers[i]);
			Memory::Free(managers[i]);
		}
		delete[] managers;
	}

	Result UIManager::SetElementTypeRegistry(const UIElementTypeRegistry& registry)
	{
		if (typeCount != 0)
			return BLAZE_ERROR_LOG("BlazeEngine", "You cant change the element type registry");

		if (registry.GetElementTypeCount() == 0)
			return BLAZE_ERROR_LOG("BlazeEngine", "You cant set the element type registry to an empty one");
		
		this->registry = registry;
		typeCount = registry.GetElementTypeCount();
		
		managers = new UIBaseElementManager * [typeCount];

		for (int i = 0; i < typeCount; ++i)
		{
			const auto& typeData = this->registry.GetElementTypeData(i);

			managers[i] = (UIBaseElementManager*)Memory::Allocate(typeData.managerSize);
			typeData.constructManager(managers[i]);
			managers[i]->manager = this;
		}

		SetupElementManagers();

		return { };
	}	

	Result UIManager::AddElement(UIElement* ptr, uint typeIndex, StringView layerName)
	{		
		if (typeCount == 0)
			return BLAZE_ERROR_LOG("BlazeEngine", "No element type registry set");

		if (ptr->tiedParent != nullptr)
			return BLAZE_ERROR_LOG("BlazeEngine", "Trying to add a tied element");

		auto layerIT = layerNames.find(layerName);

		if (layerIT == layerNames.end())
			return BLAZE_ERROR_LOG("BlazeEngine", "Trying to ad a element to a layer that doesn't exist");			

		auto& layer = *layerIT->second;

		ptr->manager = this;
		ptr->layer = layer.name;
		ptr->typeIndex = typeIndex;
		
		layer.elements.push_back(ptr);

		ptr->AttachedToManager();

		for (auto el : ptr->tiedElements)
		{
			el->manager = this;
			el->AttachedToManager();
		}

		return Result();
	}
	Result UIManager::RemoveElement(UIElement* ptr)
	{		
		if (ptr->manager != this)
			return Result(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE,
				"BlazeEngine", "Trying to remove a element from a manager that it doesn't belong to"));

		for (auto el : ptr->tiedElements)
		{
			el->manager = nullptr;
			el->DetachedFromManager();
		}

		ptr->DetachedFromManager();

		if (ptr == focusedElement)
			focusedElement = nullptr;
		
		auto& arrayElements = layerNames[ptr->layer]->elements;
		arrayElements.erase(std::find(arrayElements.begin(), arrayElements.end(), ptr));		

		ptr->layer = "";
		ptr->manager = nullptr;
		ptr->typeIndex = -1;

		return Result();
	}

	void UIManager::AddEventToQueue(const UIEvent& event)
	{
		if (event.functions.size() > 0)
			eventQueue.push(&event);
	}

	Result UIManager::CreateLayer(StringView name)
	{
		auto layerIT = layers.emplace(layers.end());
		auto& layer = *layerIT;

		if (!layerNames.insert({ name, layerIT }).second)
		{
			layers.erase(layerIT);

			return Result(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE,
				"BlazeEngine", "Trying to create a already existing layer"));
		}

		layer.name = name;

		return Result();
	}
	Result UIManager::CreateLayerAfter(StringView name, StringView relative)
	{
		auto relativeMapIT = layerNames.find(relative);

		if (relativeMapIT == layerNames.end())
			return Result(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE,
				"BlazeEngine", "Trying to create a layer relative to a non existing layer"));
		
		auto layerIT = layers.emplace(++relativeMapIT->second);
		auto& layer = *layerIT;

		if (!layerNames.insert({ name, layerIT }).second)
		{
			layers.erase(layerIT);

			return Result(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE,
				"BlazeEngine", "Trying to create a already existing layer"));
		}

		layer.name = name;		

		return Result();
	}
	Result UIManager::CreateLayerBefore(StringView name, StringView relative)
	{
		auto relativeMapIT = layerNames.find(relative);

		if (relativeMapIT == layerNames.end())
			return Result(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE,
				"BlazeEngine", "Trying to create a layer relative to a non existing layer"));

		auto layerIT = layers.emplace(relativeMapIT->second);
		auto& layer = *layerIT;

		if (!layerNames.insert({ name, layerIT }).second)
		{
			layers.erase(layerIT);

			return Result(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE,
				"BlazeEngine", "Trying to create a already existing layer"));
		}

		layer.name = name;		

		return Result();
	}

	bool UIManager::HasLayer(StringView layer)
	{
		return layerNames.find(layer) != layerNames.end();
	}

	Result UIManager::Render()
	{
		Mat4f vp2d = Math::OrthographicMatrix<float>(0, viewport.size.x, 0, viewport.size.y, -1000, 1000);		

		for (auto& layer : layers)
			for (auto& element : layer.elements)
				if (!element->flags.test(4))
					managers[element->typeIndex]->Render(element);

		return Result();
	}
	Result UIManager::Update()
	{
		Vec2i mp = Input::GetMousePos() - viewport.pos;
		bool focused = Input::GetFocusedWindow() != nullptr;

		++updateState;
		blockingElement = nullptr;
		
		for (auto it = layers.rbegin(); it != layers.rend(); ++it)
			for (auto elIT = it->elements.rbegin(); elIT != it->elements.rend(); ++elIT)
			{
				auto element = *elIT;

				CleanElement(element);
			}

		for (auto it = layers.rbegin(); it != layers.rend(); ++it)
			for (auto elIT = it->elements.rbegin(); elIT != it->elements.rend(); ++elIT)
			{
				auto element = *elIT;
				UIBaseElementManager* elementManager = managers[element->typeIndex];

				Vec2f pos = element->viewportPos;
				Vec2f size = element->size;

				if (!element->flags.test(4))
					elementManager->Update(element);
				
				if (focused && blockingElement == nullptr && element->IsSolid() && element->IsActive() &&
					mp.x >= pos.x && mp.x < pos.x + size.x &&
					mp.y >= pos.y && mp.y < pos.y + size.y)
					blockingElement = element;

			}

		std::queue<const UIEvent*> oldQueue;
		eventQueue.swap(oldQueue);

		while (!oldQueue.empty())
		{
			for (auto f : oldQueue.front()->functions)
				f();

			oldQueue.pop();
		}

		return Result();
	}

	void UIManager::DrawDebugLines() const
	{		
		static std::unordered_map<const UIManager*, std::vector<ColorRGBAf>> layerColorsMap;
		auto& layerColors = layerColorsMap[this];

		if (layerColors.size() != layers.size())
		{
			size_t oldSize = layerColors.size();
			layerColors.resize(layers.size());

			for (int i = oldSize; i < layerColors.size(); ++i)
				layerColors[i] = Random::ColorRGBAf();
		}

		uint buttonType = registry.GetElementTypeIndex("Button");


		uint k = 0;
		for (auto& layer : layers)
		{
			ColorRGBAf layerColor = layerColors[k];

			for (auto element : layer.elements)
			{
				if (!element->IsActive())
					continue;

				auto data = managers[element->typeIndex]->GetDebugRenderData(element);
				for (const auto& dElement : data.elements)
				{
					ColorRGBAf color = layerColor;
					if (dElement.overrideLayerColor)
						color = dElement.color;

					Vec2f p1 = dElement.rect.pos + (Vec2f)viewport.pos;
					Vec2f p2 = p1 + dElement.rect.size;
					Graphics::DrawBoxOutline2D(p1, p2, color, dElement.thickness);
				}
			}

			++k;
		}
	}

	bool UIManager::TakeFocus(UIElement* element)
	{
		if (IsFocusTaken())
			return false;
		focusedElement = element;
		return true;
	}
	void UIManager::ReturnFocus()
	{
		focusedElement = nullptr;
	}

	void UIManager::SetViewport(Vec2i pos, Vec2i size)
	{
		viewport = { pos, size };
		Vec2f screenVP = (Vec2f)Renderer::GetViewportSize();
		vp2d = Math::OrthographicMatrix<float>(0, screenVP.x, 0, screenVP.y, -1000, 1000) * 
			Math::ScalingMatrix(Vec3f(1, 1, 1))* Math::TranslationMatrix(Vec3f(pos.x, pos.y, 0));
		//vp2d = Math::OrthographicMatrix<float>(+pos.x, +pos.x + size.x, +pos.y, +pos.y + size.y, -1000, 1000);

		AddEventToQueue(viewportChanged);
	}

	void UIManager::AddViewportChangedFunction(UIEventFunction function)
	{
		viewportChanged += function;
	}	

	UIBaseElementManager& UIManager::GetElementManager(uint typeIndex)
	{
		return *managers[typeIndex];
	}	
}