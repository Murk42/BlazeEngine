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

#define BLAZE_ENGINE_DEBUG(message) ::Blaze::Debug::Logger::LogDebug("Blaze Engine", message)
#define BLAZE_ENGINE_INFO(message) ::Blaze::Debug::Logger::LogInfo("Blaze Engine", message)
#define BLAZE_ENGINE_WARNING(message) ::Blaze::Debug::Logger::LogWarning("Blaze Engine", message)
#define BLAZE_ENGINE_ERROR(message) ::Blaze::Debug::Logger::LogError("Blaze Engine", message)
#define BLAZE_ENGINE_FATAL(message) ::Blaze::Debug::Logger::LogFatal("Blaze Engine", message)