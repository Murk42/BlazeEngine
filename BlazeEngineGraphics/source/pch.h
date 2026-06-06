#pragma once

#include "BlazeEngine/Core/Debug/Logger.h"

#if not (defined(BLAZE_GRAPHICS_OPENGL))
#define BLAZE_GRAPHICS_OPENGL
#endif

#ifdef BLAZE_GRAPHICS_OPENGL
#include <gl/glew.h>
#endif