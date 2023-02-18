#pragma once
#include <string>
#include "Command.h"

class Library
{
	void* handle;
public:
	Library();
	Library(Library&&);
	~Library();

	void FreeLibrary();
	Result LoadLibrary(std::string path);
	Result GetFunction(std::string name, void*& ptr);
	template<typename T>
	Result GetFunction(std::string name, T& ptr)
	{
		void* _ptr;
		Result r = GetFunction(name, _ptr);
		ptr = (T)_ptr;
		return r;
	}

	Library& operator=(Library&&) noexcept;
};