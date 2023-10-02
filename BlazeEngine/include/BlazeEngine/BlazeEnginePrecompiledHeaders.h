#pragma once

//---Engine PCH---
#include <type_traits>
#include <utility>
#include <functional>
#include <memory>
#include <cstdio>
#include <filesystem>
#include <concepts>
#include <array>
#include <string_view>

#include "BlazeEngine/Core/EngineCore.h"

#include "BlazeEngine/DataStructures/Common.h"

#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/DataStructures/StringView.h"
#include "BlazeEngine/DataStructures/StringUTF8.h"
#include "BlazeEngine/DataStructures/StringViewUTF8.h"
#include "BlazeEngine/DataStructures/StringHash.h"

#include "BlazeEngine/Utilities/Time.h"

#include "BlazeEngine/Math/Vector.h"
#include "BlazeEngine/Math/Quaternion.h"
#include "BlazeEngine/Math/Matrix.h"
#include "BlazeEngine/Math/Math.h"

#include "BlazeEngine/File/Path.h" //needs String

#include "BlazeEngine/Debug/Callstack.h" //needs Path, String, uint
#include "BlazeEngine/Debug/Log.h" //needs String, TimePoint, uint, Callstack
#include "BlazeEngine/Debug/Result.h" //needs Log, String, uint
#include "BlazeEngine/Debug/LoggerListener.h"
#include "BlazeEngine/Debug/ResultValue.h"
#include "BlazeEngine/Debug/Logger.h"

#include "BlazeEngine/Memory/MemoryManager.h"
#include "BlazeEngine/Memory/Allocator.h"

#include "BlazeEngine/DataStructures/Hash.h"


#include "BlazeEngine/DataStructures/Array.h"
#include "BlazeEngine/DataStructures/ArrayImpl.h"
#include "BlazeEngine/DataStructures/Map.h"
#include "BlazeEngine/DataStructures/MapImpl.h"
#include "BlazeEngine/DataStructures/VirtualMap.h"
#include "BlazeEngine/DataStructures/VirtualMapImpl.h"
#include "BlazeEngine/DataStructures/Set.h"
#include "BlazeEngine/DataStructures/SetImpl.h"
#include "BlazeEngine/DataStructures/List.h"
#include "BlazeEngine/DataStructures/ListImpl.h"
#include "BlazeEngine/DataStructures/DualList.h"
#include "BlazeEngine/DataStructures/DualListImpl.h"
#include "BlazeEngine/DataStructures/UnorderedArray.h"
#include "BlazeEngine/DataStructures/UnorderedArrayImpl.h"

#include "BlazeEngine/DataStructures/Handle.h"
#include "BlazeEngine/DataStructures/HandleImpl.h"

#include "BlazeEngine/DataStructures/ArrayView.h"

#include "BlazeEngine/Utilities/StringParsing.h"
//----------------