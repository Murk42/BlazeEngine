#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/StringView.h"
#include <vector>

namespace Blaze
{
	class ByteStream;

	namespace File
	{
		bool BLAZE_API Read(StringView path, ByteStream* ptr, bool emitLogOnError = true);
		bool BLAZE_API Write(StringView path, bool clear, ByteStream* ptr, bool emitLogOnError = true);
		bool BLAZE_API Create(StringView path, ByteStream* ptr);
		bool BLAZE_API Delete(StringView path);
		bool BLAZE_API Exists(StringView path);
	};
}