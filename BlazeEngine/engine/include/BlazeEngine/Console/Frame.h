#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/DataStructures/Types.h"
#include "BlazeEngine/DataStructures/Vector.h"

namespace Blaze
{
	namespace Console
	{
		class BLAZE_API Frame
		{
			void* ptr;
			Vec2i cursor;
			Vec2i size;
			Vec2i pos;
			byte color;

			Frame(const Frame&) = delete;
			void operator=(const Frame&) = delete;

			size_t Apply(const char*);
			void WriteRaw(const char*, size_t);
		public:			
			Frame();
			Frame(const Vec2i& pos, const Vec2i& size);
			Frame(Frame&&) noexcept;
			~Frame();

			void Write(const String& s, const Vec2i& pos);
			void Write(const String& s);
			void Write(const char*, size_t);
			void MoveCursor(const Vec2i& pos);

			void Clear();
			void Refresh();

			void SetSize(Vec2i size);
			void SetPos(Vec2i pos);

			inline Vec2i GetSize() const { return size; }
			inline Vec2i GetPos() const { return pos; }

			void operator=(Frame&&) noexcept;

			friend class Console;
		};
	}
}