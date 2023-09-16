#pragma once
#if defined(GRAPHICS_OPENGL)
#include "TextRenderer_OpenGL.h"
#elif defined(GRAPHICS_VULKAN)
#include "TextRenderer_Vulkan.h"
#else
#include "TextRenderer_Dynamic.h"
#endif