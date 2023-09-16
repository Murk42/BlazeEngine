#pragma once
#if defined(GRAPHICS_OPENGL)
#include "EmptyRenderer_OpenGL.h"
#elif defined(GRAPHICS_VULKAN)
#include "EmptyRenderer_Vulkan.h"
#else
#include "EmptyRenderer_Dynamic.h"
#endif