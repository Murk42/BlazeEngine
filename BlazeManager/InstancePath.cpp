#include "InstancePath.h"
#include <direct.h>
#include <filesystem>

InstancePath::InstancePath(std::string newPath)
{
	old = std::filesystem::current_path().string();
	_chdir(newPath.c_str());
}

InstancePath::~InstancePath()
{
	_chdir(old.c_str());
}
