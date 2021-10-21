#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
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
			StaticStorage,
			DynamicStorage,
			ClientStorage,
			MapRead,
			MapWrite,
			MapPersistant,
			MapCoherent,
		};

		ENUM_CLASS_BITWISE_OPERATIONS(GraphicsBufferStaticStorageHint)

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

			inline uint GetHandle() const { return id; }

			GraphicsBuffer& operator=(const GraphicsBuffer&) = delete;
			GraphicsBuffer& operator=(GraphicsBuffer&&) noexcept;			

			friend class VertexArray;			
		};
	}
}