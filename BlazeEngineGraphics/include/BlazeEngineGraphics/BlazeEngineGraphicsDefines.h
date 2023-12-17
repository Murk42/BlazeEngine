#pragma once

#ifdef BLAZE_STATIC
#define BLAZE_GRAPHICS_API
#else
#define BLAZE_GRAPHICS_API __declspec(dllexport)
#endif