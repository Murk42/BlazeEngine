#pragma once

#ifdef BLAZE_STATIC
#define BLAZE_GRAPHICS_API
#else
#define BLAZE_GRAPHICS_API __declspec(dllexport)
#endif

#define BLAZE_ENGINE_GRAPHICS_DEBUG(message, ...) ::Blaze::Debug::Logger::LogDebug("Blaze Engine Graphics", message, __VA_ARGS__)
#define BLAZE_ENGINE_GRAPHICS_INFO(message, ...) ::Blaze::Debug::Logger::LogInfo("Blaze Engine Graphics", message, __VA_ARGS__)
#define BLAZE_ENGINE_GRAPHICS_WARNING(message, ...) ::Blaze::Debug::Logger::LogWarning("Blaze Engine Graphics", message, __VA_ARGS__)
#define BLAZE_ENGINE_GRAPHICS_ERROR(message, ...) ::Blaze::Debug::Logger::LogError("Blaze Engine Graphics", message, __VA_ARGS__)
#define BLAZE_ENGINE_GRAPHICS_FATAL(message, ...) ::Blaze::Debug::Logger::LogFatal("Blaze Engine Graphics", message, __VA_ARGS__)