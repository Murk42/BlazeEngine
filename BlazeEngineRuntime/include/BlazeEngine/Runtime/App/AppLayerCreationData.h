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
		AppLayerCreationData(AppLayerCreationData&& other) noexcept = default;

		void Clear();

		template<IsDerivedFrom<AppLayer> T, typename ... Args> requires IsConstructibleFrom<T, Args...>
		void Initialize(Args&& ... args);

		Handle<AppLayer> CreateLayer();

		AppLayerCreationData& operator=(const AppLayerCreationData& other) = delete;
		AppLayerCreationData& operator=(AppLayerCreationData&& other) noexcept = default;

		template<IsDerivedFrom<AppLayer> T, typename ... Args> requires IsConstructibleFrom<T, Args...>
		static AppLayerCreationData New(Args&& ... args);
	private:
		using LayerConstructFunction = Handle<AppLayer>(*)(VirtualTupleBase& arguments);

		LayerConstructFunction constructFunction;
		Handle<VirtualTupleBase> arguments;
	};

	template<IsDerivedFrom<AppLayer> T, typename ...Args> requires IsConstructibleFrom<T, Args...>
	inline void AppLayerCreationData::Initialize(Args&& ...args)
	{
		constructFunction = [](VirtualTupleBase& arguments) {
			return apply([&](auto&& ... args) { return Handle<AppLayer>::CreateDerived<T>(std::forward<Args>(args)...); }, std::move(static_cast<Tuple<Args...>&>(static_cast<VirtualTuple<Args...>&>(arguments))));
			};
		arguments = Handle<VirtualTupleBase>::CreateDerived<VirtualTuple<Args...>>(std::forward<Args>(args)...);
	}
	template<IsDerivedFrom<AppLayer> T, typename ...Args> requires IsConstructibleFrom<T, Args...>
	inline AppLayerCreationData AppLayerCreationData::New(Args && ...args)
	{
		AppLayerCreationData out;
		out.Initialize<T>(std::forward<Args>(args)...);
		return out;
	}
}