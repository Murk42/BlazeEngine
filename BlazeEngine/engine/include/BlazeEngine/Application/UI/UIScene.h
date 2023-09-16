#pragma once
#include "BlazeEngine/Application/UI/UIEntityView.h"
#include "BlazeEngine/Application/UI/UIComponent.h"
#include "BlazeEngine/Application/UI/UISystem.h"
#include "BlazeEngine/Application/ECS/Scene.h"

namespace Blaze::UI
{		
	namespace Components
	{
		class UIElementComponent;		
	}

	template<typename T>
	concept IsUIComponent = ECS::IsComponent<T> && std::derived_from<T, UIComponent> && std::derived_from<typename T::template System, UISystem>;

	class BLAZE_API UIScene
	{
	public:
		struct BLAZE_API Layer
		{
			struct Element
			{
				Components::UIElementComponent* element;
				Components::UITransformComponent* transform;
				ECS::Entity* entity;
			};

			String name;
			DualList<Element> elements;

			Layer();
			~Layer();
		};

		UIScene();
		~UIScene();

		Result SetECSScene(ECS::Scene* scene);
		ECS::Scene* GetECSScene() const;

		const ECS::ComponentTypeRegistry& GetRegistry() const;

		Result CreateLayer(StringView name);
		Result CreateLayerAfter(StringView name, StringView relative);
		Result CreateLayerBefore(StringView name, StringView relative);
		bool HasLayer(StringView layer);
		
		template<typename ... Cs> requires (ECS::IsComponent<Cs> && ...)
		UIEntityView<Cs...> Create(StringView name, StringView layerName);
		ECS::Entity* Create(StringView name, StringView layerName, ArrayView<const ECS::ComponentTypeData*> copmonentsTypeData);		
		Result Destroy(Components::UIElementComponent* element);
		ECS::Entity* GetElement(StringView name) const;

		Result MoveToFirstElementInLayer(Components::UIElementComponent* element);
		Result MoveToLastElementInLayer(Components::UIElementComponent* element);
		Result MoveAfterElementInLayer(Components::UIElementComponent* element, Components::UIElementComponent* relative);
		Result MoveBeforeElementInLayer(Components::UIElementComponent* element, Components::UIElementComponent* relative);
		Result MoveOneAfterInLayer(Components::UIElementComponent* element);
		Result MoveOneBeforeInLayer(Components::UIElementComponent* element);

		Result RenderScene();
		Result UpdateScene();
		Result RenderDebugData();

		template<typename C> requires IsUIComponent<C>
		Result UpdateSystem();
		Result UpdateSystem(const ECS::ComponentTypeData&);		

		template<typename C> requires IsUIComponent<C>
		UISystem* GetSystem();
		UISystem* GetSystem(const ECS::ComponentTypeData&);

		void Clear();

		bool TakeFocus(Components::UIElementComponent* focusedElement);
		void ReturnFocus();		
		bool IsFocusTaken();
		Components::UIElementComponent* GetFocusedElement();

		bool IsMouseBlocked() const;
		Components::UIElementComponent* GetBlockingElement() const;		

		auto& GetLayers() { return layers; }
		const auto& GetLayers() const { return layers; }
	private:
		ECS::Scene* ecsScene;				

		Map<String, Components::UIElementComponent*> nameToElementMap;
		Map<String, DualList<Layer>::Iterator> layerNames;
		DualList<Layer> layers;

		Components::UIElementComponent* focusedElement;
		Components::UIElementComponent* blockingElement;

		uint32 updateState;		

		Result UpdateSystemUnsafe(const ECS::ComponentTypeData&);
		UISystem* GetSystemUnsafe(const ECS::ComponentTypeData&);

		Result PrepareElementCreation(StringView name, StringView layer);
		Result FinishElementCreation(Components::UIElementComponent* element);
		
		Result ResolveSize(Components::UITransformComponent* transform);
		Result ResolvePosition(Components::UITransformComponent* transform);		
	};	
	
	template<typename ... Cs> requires (ECS::IsComponent<Cs> && ...)
	UIEntityView<Cs...> UIScene::Create(StringView name, StringView layerName)
	{
		PrepareElementCreation(name, layerName);

		auto entityView = ecsScene->Create<Components::UITransformComponent, Components::UIElementComponent, Cs...>();
		auto element = entityView.GetComponent<1>();

		FinishElementCreation(element);

		return UIEntityView<Cs...>(entityView.GetEntity());
	}
	template<typename C> requires IsUIComponent<C>
	inline Result UIScene::UpdateSystem()
	{
		const ECS::ComponentTypeData* typeData;
		if (!GetRegistry().GetComponentTypeData<C>(typeData))
			return BLAZE_ERROR_RESULT("Blaze Engine", "Component type is not in registry");

		return UpdateSystemUnsafe(*typeData);		
	}
	template<typename C> requires IsUIComponent<C>
	inline UISystem* UIScene::GetSystem()
	{
		const ECS::ComponentTypeData* typeData;
		if (!GetRegistry().GetComponentTypeData<C>(typeData))
			Debug::Logger::LogError("Blaze Engine", "Component type is not in registry");

		return GetSystemUnsafe(*typeData);		
	}
}