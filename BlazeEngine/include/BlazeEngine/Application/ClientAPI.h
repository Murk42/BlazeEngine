#pragma once

#ifdef BLAZE_STATIC
#define CLIENT_API __declspec(dllexport)
#define BLAZE_API
#else
#define CLIENT_API extern "C" __declspec(dllexport)
#define BLAZE_API __declspec(dllexport)
#endif