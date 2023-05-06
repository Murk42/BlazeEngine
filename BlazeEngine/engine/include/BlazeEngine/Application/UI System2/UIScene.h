#pragma once
#include "BlazeEngine/Application/ECS/Scene.h"
#include "BlazeEngine/DataStructures/Rect.h"
#include "BlazeEngine/DataStructures/Matrix.h"
#include <concepts>

namespace Blaze::UI2
{	

	class UIRenderer;
	class UIElement;

	class BLAZE_API UIScene
	{
	public:
		struct Layer
		{
			String name;
			std::list<UIElement*> elements;			
		};

		UIScene();
		~UIScene();

		Result SetECSScene(ECS::Scene* scene);
		ECS::Scene* GetECSScene() const;

		Result CreateLayer(StringView name);
		Result CreateLayerAfter(StringView name, StringView relative);
		Result CreateLayerBefore(StringView name, StringView relative);
		bool HasLayer(StringView layer);
		
		UIElement* CreateElement(const ECS::ComponentTypeData& typeData, StringView name, StringView layerName);		
		template<typename T, typename ... Args> T* CreateElement(StringView name, StringView layerName, Args&& ... args);

		Result DeleteElement(UIElement* element);
		ECS::Entity* GetElement(StringView name) const;

		Result MoveToFirstElementInLayer(UIElement* element);
		Result MoveToLastElementInLayer(UIElement* element);
		Result MoveAfterElementInLayer(UIElement* element, UIElement* relative);
		Result MoveBeforeElementInLayer(UIElement* element, UIElement* relative);
		Result MoveOneAfterInLayer(UIElement* element);
		Result MoveOneBeforeInLayer(UIElement* element);

		Result RenderScene();
		Result UpdateScene();

		bool TakeFocus(UIElement* focusedElement);
		void ReturnFocus();

		void SetViewport(Vec2i pos, Vec2i size);		

		bool IsFocusTaken();
		UIElement* GetFocusedElement();

		bool IsMouseBlocked() const;
		UIElement* GetBlockingElement() const;

		inline Recti GetViewport() const;		
		inline const Mat4f& GetProjectionMatrix() const;

		auto& GetLayers() { return layers; }
		const auto& GetLayers() const { return layers; }
	private:
		ECS::Scene* ecsScene;				

		std::unordered_map<String, UIElement*> nameToElementMap;
		std::unordered_map<String, std::list<Layer>::iterator> layerNames;
		std::list<Layer> layers;

		UIElement* focusedElement;
		UIElement* blockingElement;

		uint32 updateState;

		Recti viewport;		
		Mat4f vp2d;		

		UIElement* CreateElement(const ECS::ComponentTypeData& typeData, StringView name, StringView layerName, const std::function<void(void*)>& function);		

		friend class UIRenderer;
	};

	//template<typename T> 
	//T* UIScene::CreateElement(StringView layerName)
	//{
	//	return (T*)CreateElement(ecsScene->GetRegistry().GetComponentTypeData<T>(), layerName);
	//}
	//template<typename T>
	//T* UIScene::CreateElement(StringView name, StringView layerName)
	//{
	//	return (T*)CreateElement(ecsScene->GetRegistry().GetComponentTypeData<T>(), name, layerName);
	//}
	template<typename T, typename ...Args>
	inline T* UIScene::CreateElement(StringView name, StringView layerName, Args && ...args)
	{		
		return (T*)CreateElement(ecsScene->GetRegistry().GetComponentTypeData<T>(), name, layerName, [&](void* ptr) {
			new (ptr) T(std::forward<Args>(args)...);
			});
	}
}