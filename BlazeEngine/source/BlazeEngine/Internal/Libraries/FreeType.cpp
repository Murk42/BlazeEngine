#include "pch.h"
#include "BlazeEngine/Internal/Libraries/FreeType.h"
#include "BlazeEngineCore/DataStructures/StringView.h"
#include "BlazeEngineCore/Debug/Logger.h"

namespace Blaze
{
	static FT_Library freeTypeLibrary;

	FT_Library GetFreeTypeLibrary()
	{
		return freeTypeLibrary;
	}

	static StringView GetFTError(FT_Error error)
	{
		const char* ptr = FT_Error_String(error);
		uintMem len = strlen(ptr);
		return StringView(ptr, len);
	}

#define _STRINGIZE(x) #x
#define STRINGIZE(x) _STRINGIZE(x)

	Timing InitializeCoreFreeType()
	{
		Timing timing{ "FreeType initialization" };	

		if (auto error = FT_Init_FreeType(&freeTypeLibrary))
			BLAZE_ENGINE_CORE_FATAL("Failed to initialize the FreeType libary. FT_Error_String returned \"" + GetFTError(error) + "\"");
		
		BLAZE_ENGINE_CORE_INFO("Successfully initialized FreeType " STRINGIZE(FREETYPE_MAJOR) "." STRINGIZE(FREETYPE_MINOR) "." STRINGIZE(FREETYPE_PATCH));

		return timing;
	}

	TimingResult InitializeFreeType()
	{
		Timing timing{ "FreeType" };
		
		timing.AddNode(InitializeCoreFreeType());

		return timing;
	}
	void TerminateFreeType()
	{
		FT_Done_FreeType(freeTypeLibrary);
	}

	static unsigned long FreeTypeStreamIOFunc(FT_Stream ftStream, unsigned long offset, unsigned char* buffer, unsigned long count)
	{
		ReadStream& stream = *(ReadStream*)ftStream->pathname.pointer;

		if (count == 0)
			return stream.SetPosition(offset) ? 0 : 1; //The function should return non-zero if it fails. SetPosiiton returns true on success.
		else
		{
			stream.SetPosition(offset);
			ftStream->pos = offset;

			return stream.Read(buffer, count);
		}
	}
	static void FreeTypeStreamCloseFunc(FT_Stream ftStream)
	{

	}
	FT_Face OpenFaceFromStream(ReadStream& source, int index)
	{
		FT_StreamRec_ stream{
			.base = nullptr,
			.size = (unsigned long)source.GetSize(),
			.pos = (unsigned long)source.GetPosition(),
			.descriptor = {.pointer = nullptr },
			.pathname = {.pointer = &source },
			.read = FreeTypeStreamIOFunc,
			.close = FreeTypeStreamCloseFunc,
			.memory = nullptr,
			.cursor = nullptr,
			.limit = nullptr,
		};
		FT_Open_Args openArgs{
			.flags = FT_OPEN_STREAM,
			.memory_base = nullptr,
			.memory_size = 0,
			.pathname = nullptr,
			.stream = &stream,
			.driver = NULL,
			.num_params = 0,
			.params = nullptr
		};

		FT_Face ptr = nullptr;
		if (auto error = FT_Open_Face(GetFreeTypeLibrary(), &openArgs, index, &ptr))
		{
			BLAZE_ENGINE_ERROR("Failed to open font face with index {}, returned FreeType error code {}", index, error);
			ptr = nullptr;
		}

		return ptr;
	}
}