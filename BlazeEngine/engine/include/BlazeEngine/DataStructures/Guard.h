#pragma once
#include <algorithm>

namespace Blaze
{
	template<typename T, bool array_guard = false>
	class Guard;

	template<typename T>
	class Guard<T, false>
	{
		Guard(const Guard<T, false>&) = delete;
		void operator=(const Guard<T, false>&) = delete;
	public:
		T* ptr;

		Guard() : ptr(nullptr) { }
		Guard(Guard<T, false>&& g) : ptr(g.ptr) { g.ptr = nullptr; }
		Guard(T* ptr) : ptr(ptr) { }
		~Guard()
		{
			delete ptr;
		}

		inline T* operator->() const { return ptr; }

		void operator=(T* ptr)
		{
			delete this->ptr;
			this->ptr = ptr;
		}
		void operator=(Guard<T, false>&& g)
		{
			T* buf = ptr;
			ptr = g.ptr;
			g.ptr = buf;			
		}
	};

	template<typename T>
	class Guard<T, true>
	{
		Guard(const Guard<T, true>&) = delete;
		void operator=(const Guard<T, true>&) = delete;
	public:
		T* ptr;

		Guard() : ptr(nullptr) { }
		Guard(Guard<T, true>&& g) : ptr(g.ptr) { g.ptr = nullptr; }
		Guard(T* ptr) : ptr(ptr) { }
		~Guard()
		{
			delete[] ptr;
		}

		inline T* operator->() const { return ptr; }
		inline T& operator[](size_t i) const { return ptr[i]; }

		void operator=(T* ptr)
		{
			delete[] this->ptr;
			this->ptr = ptr;
		}
		void operator=(Guard<T, true>&& g)
		{
			T* buf = ptr;
			ptr = g.ptr;
			g.ptr = buf;
		}
	};

	template<>
	class Guard<void, true>
	{
		Guard(const Guard<void, true>&) = delete;
		void operator=(const Guard<void, true>&) = delete;
	public:
		void* ptr;

		Guard() : ptr(nullptr) { }
		Guard(Guard<void, true>&& g) noexcept : ptr(g.ptr) { g.ptr = nullptr; }
		Guard(void* ptr) : ptr(ptr) { }
		~Guard()
		{
			delete[] ptr;
		}

		inline void* operator->() const { return ptr; }		

		Guard& operator=(void* ptr)
		{
			delete[] this->ptr;
			this->ptr = ptr;
			return *this;
		}
		Guard& operator=(Guard<void, true>&& g) noexcept
		{
			void* buf = ptr;
			ptr = g.ptr;
			g.ptr = buf;
			return *this;
		}
	};
}