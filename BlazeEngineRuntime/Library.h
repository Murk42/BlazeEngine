#pragma once
#include <string>
#include "Result.h"

class Library
{
	void* handle;
public:
	Library();
	Library(Library&&);	
	~Library();

	void FreeLibrary();
	Result LoadLibrary(std::string path);
	void* GetFunction(std::string name, Result& result);

	Library& operator=(Library&&) noexcept;
};

class LibraryView
{
	void* handle;
public:
	Result Set(std::string path);
	void* GetFunction(std::string name, Result& result);
};