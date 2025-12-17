#pragma once
#include "BlazeEngine/Core/Common/VirtualTuple.h"
#include "BlazeEngine/Core/Common/Handle.h"
#include "BlazeEngine/Core/Container/VirtualList.h"
#include "BlazeEngine/Runtime/App/AppLayer.h"
#include "BlazeEngine/External/BlazeEngineCoreSTDInterface.h"

namespace Blaze
{
	class AppLayerCreationData
	{
	public:
		AppLayerCreationData();
		AppLayerCreationData(const AppLayerCreationData& other) = delete;
		AppLayerCreationData(AppLayerCreationData&& other) noexcept = default;

		template<IsDerivedFrom<AppLayer> T, typename ... Args> requires IsConstructibleFrom<T, Args...>
		void Initialize(Args&& ... args);

		bool CreateLayer(Array<Handle<AppLayer>>& layerStack) const;

		AppLayerCreationData& operator=(const AppLayerCreationData& other) = delete;
		AppLayerCreationData& operator=(AppLayerCreationData&& other) noexcept = default;
	private:
		using LayerConstructFunction = void(*)(Array<Handle<AppLayer>>&, VirtualTupleBase& arguments);

		LayerConstructFunction constructFunction;
		Handle<VirtualTupleBase> arguments;
	};

	template<IsDerivedFrom<AppLayer> T, typename ...Args> requires IsConstructibleFrom<T, Args...>
	inline void AppLayerCreationData::Initialize(Args&& ...args)
	{
		constructFunction = [](Array<Handle<AppLayer>>& stack, VirtualTupleBase& arguments) {
			apply([&](auto&& ... args) { stack.AddBack(Handle<AppLayer>::CreateDerived<T>(std::forward<Args>(args)...)); }, std::move(static_cast<Tuple<Args...>&>(static_cast<VirtualTuple<Args...>&>(arguments))));
			};
		arguments = Handle<VirtualTupleBase>::CreateDerived<VirtualTuple<Args...>>(std::forward<Args>(args)...);
	}
}