#pragma once

namespace Blaze
{
	template<typename ... T>
	class Serializer
	{
		template<typename T> struct is_vector : std::false_type { };
		template<typename T> struct is_vector<std::vector<T>> : std::true_type { };
		template<typename T> static constexpr bool is_vector_v = is_vector<T>::value;

		template<typename T>
		using pure_t = std::remove_reference_t<std::remove_const_t<T>>;
		template<typename T1>
		inline void _GetValue(size_t& offset, T1& value)
		{
			memcpy(&value, (uint8_t*)ptr + offset, sizeof(T1));
			offset += sizeof(T1);
		}
		template<typename T1>
		inline void _GetVector(size_t& offset, std::vector<T1>& value)
		{
			value.resize(*(uint32_t*)((uint8_t*)ptr + offset));
			memcpy(value.ptr(), (uint8_t*)ptr + offset + +sizeof(uint32_t), value.size() * sizeof(T1));
			offset += sizeof(uint32_t) + value.size() * sizeof(T1);
		}
		template<typename T1, typename T2>
		inline void _GetArray(size_t& offset, T1& size, T2& ptr)
		{
			memcpy(&size, (uint8_t*)ptr + offset, sizeof(T1));
			ptr = new std::remove_pointer_t<T2>[size];
			memcpy(ptr, (uint8_t*)ptr + offset + sizeof(T1), size * sizeof(T2));
			offset += sizeof(T1) + size * sizeof(std::remove_pointer_t<T2>);
		}
		template<typename T1>
		inline void _Get(size_t& offset, T1& value)
		{
			if constexpr (is_vector_v<T1>)
				_GetVector(offset, value);
			else
				_GetValue(offset, value);
		}
		template<typename T1, typename T2>
		inline void _Get(size_t& offset, T1& value1, T2& value2)
		{
			if constexpr (std::is_pointer_v<T2>)
				_GetArray<T1, T2>(offset, value1, value2);
			else
			{
				_Get<T1>(offset, value1);
				_Get<T2>(offset, value2);
			}
		}
		template<typename T1, typename T2, typename T3, typename ... T>
		inline void _Get(size_t& offset, T1& value1, T2& value2, T3& value3, T& ... values)
		{
			_Get<T1, T2>(offset, value1, value2);
			_Get<T3, T...>(offset, value3, values...);
		}

		template<typename T1>
		inline void _SetValue(size_t& offset, T1& value)
		{
			memcpy((uint8_t*)ptr + offset, &value, sizeof(T1));
			offset += sizeof(T1);
		}
		template<typename T1>
		inline void _SetVector(size_t& offset, std::vector<T1>& value)
		{
			*(size_t*)((uint8_t*)ptr + offset) = value.size();
			memcpy((uint8_t*)ptr + offset + sizeof(uint32_t), value.ptr(), value.size() * sizeof(T1));
			offset += sizeof(uint32_t) + value.size() * sizeof(T1);
		}
		template<typename T1, typename T2>
		inline void _SetArray(size_t& offset, T1& size, T2& ptr)
		{
			memcpy((uint8_t*)ptr + offset, &size, sizeof(T1));
			memcpy((uint8_t*)ptr + offset + sizeof(T1), ptr, size * sizeof(T2));
			offset += sizeof(T1) + size * sizeof(std::remove_pointer_t<T2>);
		}
		template<typename T1>
		inline void _Set(size_t& offset, T1& value)
		{
			if constexpr (is_vector_v<T1>)
				_SetVector(offset, value);
			else
				_SetValue(offset, value);
		}
		template<typename T1, typename T2>
		inline void _Set(size_t& offset, T1& value1, T2& value2)
		{
			if constexpr (std::is_pointer_v<T2>)
				_SetArray<T1, T2>(offset, value1, value2);
			else
			{
				_Set<T1>(offset, value1);
				_Set<T2>(offset, value2);
			}
		}
		template<typename T1, typename T2, typename T3, typename ... T>
		inline void _Set(size_t& offset, T1& value1, T2& value2, T3& value3, T& ... values)
		{
			_Set<T1, T2>(offset, value1, value2);
			_Set<T3, T...>(offset, value3, values...);
		}

		size_t size;
		void* ptr;
	public:
		Serializer() : ptr(nullptr), size(0) { }
		Serializer(const Serializer<T...>& s) : ptr(new uint8_t[s.size]), size(s.size) { memcpy(ptr, s.ptr, size); }
		Serializer(Serializer<T...>&& s) : ptr(std::exchange(s.ptr, nullptr)), size(s.size) { }
		Serializer(void* ptr, size_t size, bool copy)
			: size(size)
		{
			if (copy)
			{
				this->ptr = new uint8_t[size];
				memcpy(this->ptr, ptr, size);
			}
			else
				this->ptr = ptr;
		}
		~Serializer()
		{
			delete[] ptr;
		}

		void Get(T& ... args)
		{
			size_t offset = 0;
			_Get<T...>(offset, args...);
		}
		void Set(T& ... args)
		{
			size_t offset = 0;
			_Set<T...>(offset, args...);
		}

		void Resize(size_t newSize)
		{
			ptr = realloc(ptr, newSize);
			size = newSize;
		}
		inline void* Ptr() const { return ptr; }
		inline size_t Size() const { return size; }

		void operator=(const Serializer<T...>& s)
		{
			delete[] ptr;
			size = s.size;
			ptr = new uint8_t[s.size];
			memcpy(ptr, s.ptr, size);
		}
		void operator=(Serializer<T...>&& s)
		{
			ptr = std::exchange(s.ptr, nullptr);
			size = s.size;
		}
	};
}