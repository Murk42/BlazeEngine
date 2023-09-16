#pragma once
#if defined(GRAPHICS_OPENGL)
#include "../OpenGL/API_OpenGL.h"
#elif defined(GRAPHICS_VULKAN)
#include "../Vulkan/API_Vulkan.h"
#else
#include "API_dynamic.h"
#endif

namespace Graphics3
{		
#if defined (GRAPHICS_OPENGL)
	using namespace OpenGL;
#elif defined (GRAPHICS_VULKAN)
	using namespace Vulkan;
#else
	using namespace Dynamic;
#endif
}