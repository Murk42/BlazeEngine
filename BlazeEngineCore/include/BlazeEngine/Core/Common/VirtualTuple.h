#pragma once
#include "BlazeEngine/Core/Common/Tuple.h"

namespace Blaze
{
	class VirtualTupleBase
	{
	public:
		VirtualTupleBase() = default;
		VirtualTupleBase(const VirtualTupleBase&) = default;
		VirtualTupleBase(VirtualTupleBase&&) = default;
		virtual ~VirtualTupleBase() {}

		VirtualTupleBase& operator=(const VirtualTupleBase&) = default;
		VirtualTupleBase& operator=(VirtualTupleBase&&) = default;
	};
	
	template<typename ... Args>
	class VirtualTuple : public VirtualTupleBase, public Tuple<Args...>
	{
	public:
		VirtualTuple(const VirtualTuple&) = default;
		VirtualTuple(VirtualTuple&&) = default;
		VirtualTuple(Args&& ... args) : Tuple<Args...>(std::forward<Args>(args)...) {}
		~VirtualTuple() override {}

		VirtualTuple& operator=(const VirtualTuple&) = default;
		VirtualTuple& operator=(VirtualTuple&&) = default;
	};
}