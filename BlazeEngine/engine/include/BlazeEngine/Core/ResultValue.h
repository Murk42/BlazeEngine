#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Core/Result.h"

namespace Blaze
{
	template<typename T>
	class ResultValue
	{
	public:
		T value;
		Result result;

		ResultValue() = delete;
		ResultValue(const ResultValue& o) = delete;
		ResultValue(ResultValue&&) noexcept : value(std::move(o.value)), result(std::move(o.result)) { }
		ResultValue(const T& value) : value(value) { }
		ResultValue(const T& value, Result&& result) : value(value), result(std::move(result)) { }
		ResultValue(T&& value) : value(std::move(value)) { }
		ResultValue(T&& value, Result&& result) : value(std::move(value)), result(std::move(result)) { }

		operator T() const
		{
			return value;
		}		
		operator bool() const
		{
			return (bool)result;
		}

		ResultValue& operator=(const ResultValue&) = delete;
		ResultValue& operator=(ResultValue&& o) noexcept
		{
			value = std::move(o.value);
			result = std::move(o.result);
			return *this;
		}
	};
}