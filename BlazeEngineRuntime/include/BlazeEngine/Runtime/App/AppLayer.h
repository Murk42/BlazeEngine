#pragma once
#include "BlazeEngine/Runtime/IO/InputEvents.h"
#include "BlazeEngine/Core/Resource/ResourceManager.h"
#include "BlazeEngine/Runtime/App/AppRuntimeThread.h"

namespace Blaze
{
	class AppLayerCreationData;

	class AppLayer
	{
	public:
		ResourceManager& resourceManager;

		AppLayer();
		virtual ~AppLayer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void Update() {}

		virtual Input::EventProcessedState OnEvent(const Input::GenericInputEvent& event, bool processed) { return Input::EventProcessedState::NotProcessed; }		

		/*
			Adds a layer to the layer stack. The layer creation is deffered until `UpdateLayers` is called.

			Not thread safe.

			\param args - arguments to construct the layer with
		*/
		template<IsDerivedFrom<AppLayer> T, typename ... Args> requires IsConstructibleFrom<T, Args...>
		void AddLayer(Args&& ... args);
		/*
			Adds a layer to the layer stack. The layer creation is deffered until `UpdateLayers` is called.

			Not thread safe.

			\param layerCreationData - layer creation data to create the layer with layer
		*/
		void AddLayer(AppLayerCreationData&& layerCreationData);
		/*
			Removes a layer from the layer stack. The layer removal is deffered until `UpdateLayers` is called.
			The first layer that can be successfully dynamic_cast-ed to the type T will be removed.

			Not thread safe.

			\returns True if the layer was successfully found, false otherwise
		*/
		template<IsDerivedFrom<AppLayer> T>
		bool RemoveLayer();
		/*
			Removes a layer from the layer stack. The layer removal is deffered until `UpdateLayers` is called.

			Not thread safe.

			\param layer      The layer to be removed
			\returns True if the layer is part of the layer stack and will be removed, false otherwise
		*/
		bool RemoveLayer(AppLayer& layer);
		/*
			Retrieves a layer from the layer stack. The first layer that can be successfully dynamic_cast-ed to
			the type T will be retrieved. If there is no such layer nullptr is returned.

			Not thread safe.

			\returns Pointer to the layer, nullptr if the layer could not be found
		*/
		template<IsDerivedFrom<AppLayer> T>
		T* GetLayer();
	private:
		AppRuntimeThread& thread;
	};
	template<IsDerivedFrom<AppLayer> T, typename ...Args> requires IsConstructibleFrom<T, Args...>
	inline void AppLayer::AddLayer(Args && ...args)
	{
		thread.AddLayer<T, Args...>(std::forward<Args>(args)...);
	}	
    inline void AppLayer::AddLayer(AppLayerCreationData&& layerCreationData)
    {
		thread.AddLayer(std::move(layerCreationData));
    }
	template<IsDerivedFrom<AppLayer> T>
	inline bool AppLayer::RemoveLayer()
	{
		return thread.RemoveLayer<T>();
	}	
	inline bool AppLayer::RemoveLayer(AppLayer& layer)
	{
		return thread.RemoveLayer(layer);
	}
	template<IsDerivedFrom<AppLayer> T>
	inline T* AppLayer::GetLayer()
	{
		return thread.GetLayer<T>();
	}
}