#pragma once
#include "BlazeEngine/DataStructures/Common.h"
#include <vector>

namespace Blaze
{
	template<typename T>
	class Stream
	{
		T stream;
	public:
		Stream() = default;
		Stream(const Stream<T>& s) : stream(s.stream) { }
		Stream(Stream<T>&& s) : stream(std::move(s.stream)) { }
		Stream(const T& s) : stream(s) { }
		Stream(T&& s) : stream(std::move(s)) { }
		~Stream() = default;

		inline T& GetStream() { return stream; }
		inline const T& GetStream() const { return stream; }

		inline uint GetOfbset() const { stream.GetOfbset(); }
		inline size_t GetSize() const { stream.GetSize(); }
		inline std::vector<uint8>& GetData() { stream.GetData(); }
		inline const std::vector<uint8>& GetData() const { stream.GetData(); }

		template<typename V>
		inline Stream<T>& Get(V& value)
		{
			stream.Get<T>(value);
			return *this;
		}
		template<typename V>
		inline Stream<T>& Set(const V& value)
		{
			stream.Set<T>(value);
			return *this;
		}

		inline void operator=(const Stream<T>& s) { stream = s.stream; }
		inline void operator=(Stream<T>&& s) { stream = std::move(s.stream); }
		inline void operator=(const T& s) { stream = s; }
		inline void operator=(T&& s) { stream = std::move(s); }
	};
}