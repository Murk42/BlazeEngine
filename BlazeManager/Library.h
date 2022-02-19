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
	void* GetFunction(std::string name, Result& result);

	Library& operator=(Library&&) noexcept;
};