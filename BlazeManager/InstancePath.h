#pragma once
#include <string>

class InstancePath
{
	std::string old;
public:
	InstancePath(std::string newPath);
	~InstancePath();
};