#pragma once
#include "BlazeEngine/Core/Common/Handle.h"
#include "BlazeEngine/Runtime/App/AppLayer.h"
#include "BlazeEngine/Runtime/App/AppLayerCreationData.h"

namespace Blaze
{
	class Window;
	class App;

	class AppRuntimeThread
	{
	public:		
		AppRuntimeThread(Window& window);
		virtual ~AppRuntimeThread() {}

		virtual void StartRender() {}
		virtual void EndRender() {}

		void MarkForExit();

		template<IsDerivedFrom<AppLayer> T, typename ... Args> requires IsConstructibleFrom<T, Args...>
		void AddLayer(Args&& ... args);
		void AddLayer(AppLayerCreationData&& layerCreationData);
		template<IsDerivedFrom<AppLayer> T>
		void RemoveLayer();
		void RemoveLayer(AppLayer& layer);

		Window& GetWindow() { return window; }
		const Window& GetWindow() const { return window; }
	private:
		using LayerPickFunction = bool(*)(AppLayer&, void* userData);

		struct LayerRemovalData
		{
			LayerPickFunction pickFunction;
			void* userData;
		};

		Window& window;
		bool shouldExit;

		std::recursive_mutex layerTasksMutex;
		Array<AppLayerCreationData> queuedLayerCreationData;
		Array<LayerRemovalData> queuedLayerRemovalData;
		Array<Handle<AppLayer>> layerStack;

		void Run();

		void ProcessInputEvents();
		void ProcessLayerTasks();

		void RemoveLayer(const LayerRemovalData& layerRemovalData);

		friend class AppRuntimeThreadCreationData;
	};

	template<IsDerivedFrom<AppLayer> T, typename ...Args> requires IsConstructibleFrom<T, Args...>
	inline void AppRuntimeThread::AddLayer(Args&& ...args)
	{
		AppLayerCreationData creationData;
 		creationData.Initialize<T>(std::forward<Args>(args)...);
		AddLayer(std::move(creationData));
	}
	template<IsDerivedFrom<AppLayer> T>
	inline void AppRuntimeThread::RemoveLayer()
	{
		RemoveLayer({ [](AppLayer& layer, void* userData) { return dynamic_cast<T*>(&layer) != nullptr; }, nullptr});
	}
}