#pragma once
#include <type_traits>
#include <utility>
#include <functional>
#include <memory>
#include <cstdio>
#include <filesystem>
#include <concepts>
#include <array>
#include <string_view>
#include <mutex>

#include "BlazeEngineCore/BlazeEngineCoreDefines.h"

#include "BlazeEngineCore/Memory/Allocator.h"

#include "BlazeEngineCore/DataStructures/String.h"
#include "BlazeEngineCore/DataStructures/StringView.h"
#include "BlazeEngineCore/DataStructures/StringUTF8.h"
#include "BlazeEngineCore/DataStructures/StringViewUTF8.h"
#include "BlazeEngineCore/DataStructures/UnicodeChar.h"

#include "BlazeEngineCore/Utilities/Time.h"

#include "BlazeEngineCore/File/Path.h"
#include "BlazeEngineCore/File/Stream/Stream.h"

#include "BlazeEngineCore/Debug/Callstack.h"
#include "BlazeEngineCore/Debug/Log.h"
#include "BlazeEngineCore/Debug/Result.h"
#include "BlazeEngineCore/Debug/Logger.h"

#include "BlazeEngineCore/DataStructures/StringHash.h"

#include "BlazeEngineCore/DataStructures/Array.h"
#include "BlazeEngineCore/DataStructures/ArrayView.h"
#include "BlazeEngineCore/DataStructures/Hash.h"
#include "BlazeEngineCore/DataStructures/List.h"
#include "BlazeEngineCore/DataStructures/Map.h"
#include "BlazeEngineCore/DataStructures/Set.h"

#include "BlazeEngineCore/DataStructures/StringViewImpl.h"
#include "BlazeEngineCore/DataStructures/StringViewUTF8Impl.h"
#include "BlazeEngineCore/DataStructures/UnicodeCharImpl.h"
#include "BlazeEngineCore/DataStructures/ArrayImpl.h"
#include "BlazeEngineCore/DataStructures/ListImpl.h"
#include "BlazeEngineCore/DataStructures/MapImpl.h"
#include "BlazeEngineCore/DataStructures/SetImpl.h"

#include "BlazeEngineCore/Internal/GlobalData.h"