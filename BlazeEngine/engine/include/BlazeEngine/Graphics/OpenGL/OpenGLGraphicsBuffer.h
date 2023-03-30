#pragma once
#include "BlazeEngine/DataStructures/Buffer.h"

namespace Blaze
{
	namespace OpenGL
	{				
		enum class GraphicsBufferDynamicStorageHint
		{
			StreamDraw = 35040,
			StreamRead = 35041,
			StreamCopy = 35042,
			StaticDraw = 35044,
			StaticRead = 35045,
			StaticCopy = 35046,
			DynamicDraw = 35048,
			DynamicRead = 35049,
			DynamicCopy = 35050,
		};		

		enum class GraphicsBufferStaticStorageHint
		{
			StaticStorage = 0,
			DynamicStorage = 0x0100,
			ClientStorage = 0x0200,
			MapRead = 0x0001,
			MapWrite = 0x0002,
			MapPersistant = 0x00000040,
			MapCoherent = 0x00000080,
		};

		ENUM_CLASS_BITWISE_OPERATIONS(GraphicsBufferStaticStorageHint)

		enum class GraphicsBufferMapAccess
		{
			Read = 0x0001,
			Write = 0x0002,
			Persistant = 0x00000040,
			Coherent = 0x00000080,
			InvalidateRange = 0x0004,
			InvalidateBuffer = 0x0008,
			ExplicitFlush = 0x0010,
			Unsynchronized = 0x0020
		};

		ENUM_CLASS_BITWISE_OPERATIONS(GraphicsBufferMapAccess)

		class BLAZE_API GraphicsBuffer
		{			
			uint id;
		public:
			GraphicsBuffer();
			GraphicsBuffer(GraphicsBuffer&&) noexcept;
			GraphicsBuffer(const GraphicsBuffer&) = delete;
			~GraphicsBuffer();			

			void AllocateDynamicStorage(BufferView buffer, GraphicsBufferDynamicStorageHint usage);
			void AllocateStaticStorage(BufferView buffer, GraphicsBufferStaticStorageHint usage = (GraphicsBufferStaticStorageHint)0);

			void ChangeData(BufferView buffer, size_t offset);			
			void* MapBufferRange(size_t offset, size_t size, GraphicsBufferMapAccess access);
			void FlushBufferRange(size_t offset, size_t size);

			inline uint GetHandle() const { return id; }

			GraphicsBuffer& operator=(const GraphicsBuffer&) = delete;
			GraphicsBuffer& operator=(GraphicsBuffer&&) noexcept;			

			friend class VertexArray;			
		};
	}
}