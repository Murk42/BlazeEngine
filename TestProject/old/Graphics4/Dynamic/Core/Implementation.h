#pragma once
#include "DynamicImplementation.h"

namespace Graphics4
{
	enum class ImplementationAPI
	{
		Vulkan,
		OpenGL,

		Other
	};

	struct ImplementationInfo
	{
		ImplementationAPI api;
		String name;
	};

	ImplementationInfo* RegisterImplementation(const ImplementationInfo& info);
	ImplementationInfo* GetImplementationInfo(String name);
}