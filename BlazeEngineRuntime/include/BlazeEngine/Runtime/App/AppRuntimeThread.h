#pragma once
#include "BlazeEngine/Core/Common/Handle.h"
#include "BlazeEngine/Core/Resource/ResourceManager.h"
#include "BlazeEngine/Runtime/IO/InputEvents.h"
#include "BlazeEngine/Runtime/App/AppLayerCreationData.h"

namespace Blaze
{
	class AppLayer;
	class AppLayerCreationData;

	class AppRuntimeThread
	{
	public:		
		struct LayerAddedEvent
		{
			AppRuntimeThread& thread;
			AppLayer& layer;
		};
		struct LayerRemovedEvent
		{
			AppRuntimeThread& thread;
			AppLayer& layer;
		};
		EventDispatcher<LayerAddedEvent> layerAddedEventDispatcher;
		EventDispatcher<LayerRemovedEvent> layerRemovedEventDispatcher;
		ResourceManager resourceManager;

		AppRuntimeThread();
		virtual ~AppRuntimeThread();

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

		/*
			Calls `func` for all layers on the layer stack that can be successfully dynamic_cast-ed to the type T.

			Not thread safe.

			\param func     The function to be called
		*/
		template<IsDerivedFrom<AppLayer> T = AppLayer, std::invocable<T&> F>
		void ForEachLayer(const F& func);

		/*
			Removes all layers immediately, without deffering the removal.

			Not thread safe.
		*/
		void RemoveAllLayers();

		/*
			If calling this function manually, make sure that all layers are removed before the thread object is
			destroyed.
		*/
		virtual void Run() = 0;
	
		/*
			Removes all layes for which `RemoveLayer` was called. Creates all layers for which `CreateLayer` was 
			called. Calls `Update` for each layer.

			Not thread safe.
		*/
		void CreateAndRemoveDefferedLayers();

		/*
			Sends the input event to all layers on the layer stack.

			Not thread safe.
		*/
		Input::EventProcessedState SendInputEventToLayers(const Input::GenericInputEvent& event);
	private:
		Array<AppLayerCreationData> defferedLayerCreationData;
		Array<uintMem> defferedLayerRemovalData;
		Array<Handle<AppLayer>> layerStack;
	};

	template<IsDerivedFrom<AppLayer> T, typename ...Args> requires IsConstructibleFrom<T, Args...>
	inline void AppRuntimeThread::AddLayer(Args&& ...args)
	{
		AppLayerCreationData creationData;
 		creationData.Initialize<T>(std::forward<Args>(args)...);
		AddLayer(std::move(creationData));
	}
	template<IsDerivedFrom<AppLayer> T>
	inline bool AppRuntimeThread::RemoveLayer()
	{
		for (uintMem i = 0; i < layerStack.Count(); ++i)
		{
			if (T* layer = dynamic_cast<T*>(layerStack[i].Ptr()))
			{
				defferedLayerRemovalData.AddBack(i);
				return true;
			}
		}
		return false;
	}
	template<IsDerivedFrom<AppLayer> T>
	inline T* AppRuntimeThread::GetLayer()
	{
		for (auto& layer : layerStack)
			if (auto layer_specific = dynamic_cast<T*>(layer.Ptr()))
				return layer_specific;

		return nullptr;
	}
	template<IsDerivedFrom<AppLayer> T, std::invocable<T&> F>
	inline void AppRuntimeThread::ForEachLayer(const F& func)
	{
		for (auto& layer : layerStack)
			if (T* layer_specific = dynamic_cast<T*>(&*layer))
				func(*layer_specific);
	}
}