#pragma once

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