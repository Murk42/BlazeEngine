#pragma once
#if defined(GRAPHICS_OPENGL)
#include "QuadRenderer_OpenGL.h"
#elif defined(GRAPHICS_VULKAN)
#include "QuadRenderer_Vulkan.h"
#else
#include "QuadRenderer_Dynamic.h"
#endif