#pragma once
#include "BlazeEngine/Core/Common/VirtualTuple.h"
#include "BlazeEngine/Core/Common/Handle.h"

namespace Blaze
{
	template<typename T>
	class DefferedStackObject
	{
	public:
		DefferedStackObject();
		DefferedStackObject(const DefferedStackObject& other) = delete;
		DefferedStackObject(DefferedStackObject&& other) noexcept = default;

		void Clear();

		template<IsDerivedFrom<AppLayer> T, typename ... Args> requires IsConstructibleFrom<T, Args...>
		void Initialize(Args&& ... args);

		template<std::invocable<T&> F> 
		void CreateObjectAndExecute(F& function);

		DefferedStackObject& operator=(const DefferedStackObject& other) = delete;
		DefferedStackObject& operator=(DefferedStackObject&& other) noexcept = default;

		template<IsDerivedFrom<T> D, typename ... Args> requires IsConstructibleFrom<D, Args...>
		static DefferedStackObject New(Args&& ... args);
	private:
		using ConstructFunction = Handle<AppLayer>(*)(VirtualTupleBase& arguments);

		LayerConstructFunction constructFunction;
		Handle<VirtualTupleBase> arguments;
	};

	template<IsDerivedFrom<AppLayer> T, typename ...Args> requires IsConstructibleFrom<T, Args...>
	inline void AppLayerCreationData::Initialize(Args&& ...args)
	{
		constructFunction = [](VirtualTupleBase& arguments)
			{
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