#define BLAZE_ENGINE

#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Application.h"
#include "BlazeEngine/Console.h"
#include "BlazeEngine/Core.h"
#include "BlazeEngine/DataStructures.h"
#include "BlazeEngine/Event.h"
#include "BlazeEngine/File.h"
#include "BlazeEngine/Graphics.h"
#include "BlazeEngine/Input.h"
#include "BlazeEngine/Logging.h"
#include "BlazeEngine/Math.h"
#include "BlazeEngine/Resources.h"
#include "BlazeEngine/Utilities.h"
#include "BlazeEngine/Memory/MallocAllocator.h"
#include "BlazeEngine/Memory/MemoryListener.h"
#include "BlazeEngine/Memory/MemoryManager.h"

//#ifdef FINAL_BUILD
//#define CLIENT_API extern "C"
//#else
#define CLIENT_API extern "C" __declspec(dllexport)
//#endif