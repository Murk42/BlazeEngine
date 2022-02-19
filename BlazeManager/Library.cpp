#include "Library.h"
#include <Windows.h>
#include <iostream>

Library::Library()
	: handle(nullptr)
{
}

Library::Library(Library&& l)
	: handle(l.handle)
{
	l.handle = nullptr;
} 

Library::~Library()
{
	FreeLibrary();
}

void Library::FreeLibrary()
{
	if (handle != nullptr)
	{		
		::FreeLibrary((HMODULE)handle);
		handle = nullptr;
	}
}
#undef LoadLibrary
Result Library::LoadLibrary(std::string path)
{	
	FreeLibrary();

	handle = LoadLibraryA(path.c_str());
	
	if (handle == nullptr)	
		return Result("Failed to load library (error code : " + std::to_string(GetLastError()) + ") with path: " + path + "\n");				
	
	return Result();
}

void* Library::GetFunction(std::string name, Result& result)
{	
	void* ptr = GetProcAddress((HMODULE)handle, name.c_str());	

	if (ptr == nullptr)
	{
		result = Result("Failed to load library function (error code : " + std::to_string(GetLastError()) + ") with name: " + name + "\n");
		return nullptr;
	}

	return ptr;
}

Library& Library::operator=(Library&& l) noexcept
{
	FreeLibrary();
	handle = l.handle;
	l.handle = nullptr;
	return *this;
}
