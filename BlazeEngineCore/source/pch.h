#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"

#ifdef BLAZE_PLATFORM_WINDOWS
#include "BlazeEngine/Core/Internal/WindowsPlatform.h"
#include <Windows.h>
#undef DeleteFile
#undef CreateDirectory
#undef min
#undef max
#undef GetMessage
#endif

#include "BlazeEngine/Core/String/String.h"
#include "BlazeEngine/Core/String/StringView.h"
#include "BlazeEngine/Core/Debug/Logger.h"

#include "BlazeEngine/Core/BlazeEngineCoreContext.h"