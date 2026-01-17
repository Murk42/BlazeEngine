#pragma once
#include "BlazeEngine/External/BlazeEngineCoreSTDInterface.h"
#include "BlazeEngine/Core/Container/Array.h"
#include "BlazeEngine/Core/Type/TypeTraits.h"
#include "BlazeEngine/Core/Common/Tuple.h"
#include "BlazeEngine/Core/Threading/Thread.h"
#include "BlazeEngine/Runtime/App/AppRuntimeThread.h"
#include <atomic>

namespace Blaze
{
	class Thread;

	class AppRuntimeThreadCreationData
	{
	public:
		AppRuntimeThreadCreationData();
		AppRuntimeThreadCreationData(AppRuntimeThreadCreationData&& other) noexcept;
		~AppRuntimeThreadCreationData();

		void Clear();

		template<typename T, typename ... Args> requires IsConstructibleFrom<T, Args...>&& IsDerivedFrom<T, AppLayer>
		AppRuntimeThreadCreationData&& RegisterLayer(Args&& ... args);
		AppRuntimeThreadCreationData&& RegisterLayer(AppLayerCreationData&& layerCreationData);

		AppRuntimeThreadCreationData& operator=(AppRuntimeThreadCreationData&& other) noexcept;

		template<typename T, typename ... Args> requires IsConstructibleFrom<T, Args...>&& IsDerivedFrom<T, AppRuntimeThread>
		static AppRuntimeThreadCreationData New(Args&& ... args);
	private:
		struct ThreadArguments
		{
			AppRuntimeThreadCreationData& creationData;
			std::atomic_flag threadInitialized;
		};

		Thread::BasicThreadFunction threadFunction;
		Handle<VirtualTupleBase> runtimeThreadArguments;

		Array<AppLayerCreationData> layerCreationData;

		void InstantiateOnThread(Thread& thread);

		friend class App;
	};

	template<typename T, typename ...Args> requires IsConstructibleFrom<T, Args...> && IsDerivedFrom<T, AppLayer>
	inline AppRuntimeThreadCreationData&& AppRuntimeThreadCreationData::RegisterLayer(Args&& ... args)
	{
		AppLayerCreationData creationData;
		creationData.Initialize<T>(std::forward<Args>(args)...);
		RegisterLayer(std::move(creationData));
	}
	template<typename T, typename ...Args> requires IsConstructibleFrom<T, Args...> && IsDerivedFrom<T, AppRuntimeThread>
	inline AppRuntimeThreadCreationData AppRuntimeThreadCreationData::New(Args && ...args)
	{
		AppRuntimeThreadCreationData creationData;
		creationData.runtimeThreadArguments = Handle<VirtualTupleBase>::CreateDerived<VirtualTuple<Args...>>(std::forward<Args>(args)...);

		creationData.threadFunction = [](void* userData) -> int {
			AppRuntimeThreadCreationData& data = reinterpret_cast<ThreadArguments*>(userData)->creationData;
			Tuple<Args...> arguments = std::move(*static_cast<Tuple<Args...>*>(static_cast<VirtualTuple<Args...>*>(data.runtimeThreadArguments.Ptr())));
			Array<AppLayerCreationData> layerCreationData = std::move(data.layerCreationData);

			reinterpret_cast<ThreadArguments*>(userData)->threadInitialized.test_and_set();
			reinterpret_cast<ThreadArguments*>(userData)->threadInitialized.notify_one();

			T runtimeThreadDerived = make_from_tuple<T, Args...>(std::move(arguments));
			AppRuntimeThread& runtimeThread = runtimeThreadDerived;

			for (auto& layerData : layerCreationData)
				layerData.CreateLayer(runtimeThread.layerStack);
			layerCreationData.Clear(); //Release unneeded memory

			runtimeThread.Run();

			return 0;
			};

		return creationData;
	}
}