#include "BlazeEngine/Application/UI System2/UIScene.h"
#include "BlazeEngine/Application/UI System2/UIElementTransform.h"
#include "BlazeEngine/Application/UI System2/UIElement.h"
#include "BlazeEngine/Math/Math.h"
#include "BlazeEngine/Graphics/GraphicsCore.h"

#include "UIElmenetConstructData.h"

namespace Blaze::UI2
{	
	UIScene::UIScene()
		: ecsScene(nullptr), focusedElement(nullptr), blockingElement(nullptr)
	{
		SetViewport(Vec2i(), Graphics::Core::GetViewportSize());
	}
	UIScene::~UIScene()
	{		
	}

	Result UIScene::SetECSScene(ECS::Scene* scene)
	{

		if (ecsScene != nullptr)
			return BLAZE_ERROR_RESULT("Blaze Engine", "Trying to set a ECS scene to a UI scene but it already has a ECS scene set");

		ecsScene = scene;

		auto& registry = ecsScene->GetRegistry();

		if (!registry.GetComponentTypeData<UIElementTransform>())
			return BLAZE_ERROR_RESULT("Blaze Engine", "The ECS scene that you are trying to set to a UI scene does not have UIElementTransform component registered");

		if (!registry.GetComponentTypeData<UIElement>())
			return BLAZE_ERROR_RESULT("Blaze Engine", "The ECS scene that you are trying to set to a UI scene does not have UIElement component registered");

		ecsScene = scene;
		return Result();
	}

	ECS::Scene* UIScene::GetECSScene() const
	{
		return ecsScene;
	}

	Result UIScene::CreateLayer(StringView name)
	{
		auto layerIT = layers.emplace(layers.end());
		auto& layer = *layerIT;

		if (!layerNames.insert({ name, layerIT }).second)
		{
			layers.erase(layerIT);
			
			return BLAZE_WARNING_RESULT("BlazeEngine", "Trying to create a already existing layer");
		}

		layer.name = name;

		return Result();
	}
	Result UIScene::CreateLayerAfter(StringView name, StringView relative)
	{
		auto relativeMapIT = layerNames.find(relative);

		if (relativeMapIT == layerNames.end())
			return BLAZE_WARNING_RESULT("BlazeEngine", "Trying to create a layer relative to a non existing layer");

		auto layerIT = layers.emplace(++relativeMapIT->second);
		auto& layer = *layerIT;

		if (!layerNames.insert({ name, layerIT }).second)
		{
			layers.erase(layerIT);

			return BLAZE_WARNING_RESULT("BlazeEngine", "Trying to create a already existing layer");
		}

		layer.name = name;

		return Result();
	}
	Result UIScene::CreateLayerBefore(StringView name, StringView relative)
	{
		auto relativeMapIT = layerNames.find(relative);

		if (relativeMapIT == layerNames.end())
			return BLAZE_WARNING_RESULT("BlazeEngine", "Trying to create a layer relative to a non existing layer");

		auto layerIT = layers.emplace(relativeMapIT->second);
		auto& layer = *layerIT;

		if (!layerNames.insert({ name, layerIT }).second)
		{
			layers.erase(layerIT);

			return BLAZE_WARNING_RESULT("BlazeEngine", "Trying to create a already existing layer");
		}

		layer.name = name;

		return Result();
	}
	bool UIScene::HasLayer(StringView layer)
	{
		return layerNames.find(layer) != layerNames.end();
	}	

	UIElement* UIScene::CreateElement(const ECS::ComponentTypeData& typeData, StringView name, StringView layerName)
	{				
		return CreateElement(typeData, name, layerName, std::function<void(void*)>());		
	}

	Result UIScene::MoveToFirstElementInLayer(UIElement* element)
	{
		auto& elements = element->layerIt->elements;
		elements.splice(elements.begin(), elements, element->layerElementIt);
		return Result();
	}

	Result UIScene::MoveToLastElementInLayer(UIElement* element)
	{
		auto& elements = element->layerIt->elements;
		elements.splice(elements.end(), elements, element->layerElementIt);
		return Result();
	}

	Result UIScene::MoveAfterElementInLayer(UIElement* element, UIElement* relative)
	{		
		if (element->layerIt != relative->layerIt)
			return BLAZE_ERROR_RESULT("Blaze Engine", "Trying to move a element inside a layer relative to a element that isn't in the same layer");

		auto& elements = element->layerIt->elements;
		auto afterRelative = relative->layerElementIt;
		afterRelative++;

		elements.splice(afterRelative, elements, element->layerElementIt);

		return Result();
	}

	Result UIScene::MoveBeforeElementInLayer(UIElement* element, UIElement* relative)
	{				
		if (element->layerIt != relative->layerIt)
			return BLAZE_ERROR_RESULT("Blaze Engine", "Trying to move a element inside a layer relative to a element that isn't in the same layer");

		auto& elements = element->layerIt->elements;
		elements.splice(relative->layerElementIt, elements, element->layerElementIt);

		return Result();
	}

	Result UIScene::MoveOneAfterInLayer(UIElement* element)
	{		
		auto& elements = element->layerIt->elements;

		auto next = element->layerElementIt;
		++next;

		if (element->layerElementIt == elements.end())
			return Result();

		elements.splice(++next, elements, element->layerElementIt);

		return Result();
	}

	Result UIScene::MoveOneBeforeInLayer(UIElement* element)
	{		
		auto& elements = element->layerIt->elements;

		if (element->layerElementIt == elements.begin())
			return Result();

		auto before = element->layerElementIt;
		--before;
		elements.splice(before, elements, element->layerElementIt);

		return Result();
	}

	Result UIScene::DeleteElement(UIElement* element)
	{
		if (element->name.Size() > 0)
		{
			auto it = nameToElementMap.find(element->name);

			if (it == nameToElementMap.end())
				Logger::ProcessLog(BLAZE_WARNING_LOG("Blaze Engine", "Entity with name not found in the scene name list"));			
			else
				nameToElementMap.erase(it);
		}		

		layerNames.erase(element->layerIt->name);
		element->layerIt->elements.erase(element->layerElementIt);

		CHECK_RESULT(ecsScene->Destroy(element->GetEntity()));

		return Result();
	}			

	bool UIScene::TakeFocus(UIElement* element)
	{
		if (IsFocusTaken())
			return false;
		focusedElement = element;
		return true;
	}
	void UIScene::ReturnFocus()
	{
		focusedElement = nullptr;
	}

	void UIScene::SetViewport(Vec2i pos, Vec2i size)
	{
		viewport = { pos, size };
		Vec2f screenVP = (Vec2f)Graphics::Core::GetViewportSize();
		vp2d = Math::OrthographicMatrix<float>(0, screenVP.x, 0, screenVP.y, -1000, 1000) *
			Math::ScalingMatrix(Vec3f(1, 1, 1)) * Math::TranslationMatrix(Vec3f(pos.x, pos.y, 0));
		//vp2d = Math::OrthographicMatrix<float>(+pos.x, +pos.x + size.x, +pos.y, +pos.y + size.y, -1000, 1000);
	}

	bool UIScene::IsFocusTaken()
	{
		return focusedElement != nullptr;
	}

	UIElement* UIScene::GetFocusedElement()
	{
		return focusedElement;
	}

	bool UIScene::IsMouseBlocked() const
	{
		return blockingElement != nullptr;
	}

	UIElement* UIScene::GetBlockingElement() const
	{
		return blockingElement;
	}

	inline Recti UIScene::GetViewport() const
	{
		return viewport;
	}

	inline const Mat4f& UIScene::GetProjectionMatrix() const
	{
		return vp2d;
	}

	UIElement* UIScene::CreateElement(const ECS::ComponentTypeData& typeData, StringView name, StringView layerName, const std::function<void(void*)>& function)
	{
		UIElement** nameToElementItPtr = nullptr;
		if (name.Size() != 0)
		{
			auto [nameToElementIt, nameDoesntExists] = nameToElementMap.insert({ name, nullptr });

			if (!nameDoesntExists)
			{
				Logger::ProcessLog(BLAZE_ERROR_LOG("BlazeEngine", "Trying to ad a element, but an element with the same name already exists."));
				return nullptr;
			}

			uiElementConstructData.name = name;
			nameToElementItPtr = &nameToElementIt->second;			 
		}

		auto layerIt = layerNames.find(layerName);

		if (layerIt == layerNames.end())
		{
			Logger::ProcessLog(BLAZE_ERROR_LOG("BlazeEngine", "Trying to ad a element to a layer that doesn't exist"));
			return nullptr;
		}

		auto& layer = *layerIt->second;

		const auto& transformTypeData = ecsScene->GetRegistry().GetComponentTypeData<UIElementTransform>();

		uiElementConstructData.state = 1;
		uiElementConstructData.scene = this;
		uiElementConstructData.layerIt = layerIt->second;

		ECS::Component* components[2];
		std::function<void(void*)> constructors[2]{
			[&](void* ptr) {
				uiElementConstructData.transform = (UIElementTransform*)ptr;

				new (ptr) UIElementTransform();
			},
			[&](void* ptr) {
				uiElementConstructData.layerElementIt = layer.elements.emplace(layer.elements.end(), (UIElement*)ptr);

				if (function)
					function(ptr);
				else
					typeData.ConstructDirect(ptr);
			}			
		};

		ecsScene->Create({ &transformTypeData, &typeData }, components, constructors);

		auto elementComponentOffset = BaseOffset<ECS::Component, UIElement>();
		UIElement* element = (UIElement*)((uint8*)components[1] - elementComponentOffset);

		if (uiElementConstructData.state != 3)
		{
			Logger::ProcessLog(BLAZE_WARNING_LOG("Blaze Engine",
				"Internal engine error. componentsConstructData.state was changed inappropriately."));
		}

		uiElementConstructData.transform = nullptr;
		uiElementConstructData.scene = nullptr;
		uiElementConstructData.layerIt = decltype(uiElementConstructData.layerIt)();
		uiElementConstructData.layerElementIt = decltype(uiElementConstructData.layerElementIt)();
		uiElementConstructData.name = "";
		uiElementConstructData.state = 0;				

		if (name.Size() != 0)
			*nameToElementItPtr = element;

		return element;
	}

	Result UIScene::RenderScene()
	{
		for (auto& layer : layers)		
			for (auto& element : layer.elements)			
				ecsScene->GetSystem(element->GetTypeData())->Update(element);

		return Result();
	}

	Result UIScene::UpdateScene()
	{		
		auto& typeData = ecsScene->GetRegistry().GetComponentTypeData<UIElementTransform>();

		ecsScene->GetSystem(typeData)->Update(ecsScene->GetComponents(typeData));

		return Result();
	}
}