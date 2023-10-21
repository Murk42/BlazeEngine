#pragma once

#pragma warning (disable : 4996 )

#ifdef BLAZE_BUILD
#ifdef BLAZE_STATIC
#define BLAZE_API
#else
#define BLAZE_API __declspec(dllexport)
#endif

#else
#ifdef BLAZE_STATIC
#define CLIENT_API
#define BLAZE_API
#else
#define BLAZE_API __declspec(dllexport)
#define CLIENT_API extern "C" __declspec(dllexport)
#endif
#endif

#ifdef _WIN32 
#define BLAZE_PLATFORM_WINDOWS
#elif __linux__
#define BLAZE_PLATFORM_LINUX
#elif __APPLE__
#define BLAZE_PLATFORM_APPLE
#endif

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#define BLAZE_COMPILER_MSVC
#elif
#error
#endif

#ifdef _DEBUG
#define BLAZE_DEBUG
#endif

#ifdef BLAZE_DEBUG
#define BLAZE_NULL_ITERATOR_CHECK
#define BLAZE_INVALID_ITERATOR_CHECK
#endif

#ifndef BLAZE_SDL_WINDOW_DEFAULT
#define BLAZE_SDL_WINDOW_DEFAULT
#endif

#define BLAZE_OK 1
#define BLAZE_YES 1
#define BLAZE_NO 0
#define BLAZE_ERROR -1

#define BLAZE_CLASS_IMPLEMENTATION(classSize) struct { char _[classSize]; } _;