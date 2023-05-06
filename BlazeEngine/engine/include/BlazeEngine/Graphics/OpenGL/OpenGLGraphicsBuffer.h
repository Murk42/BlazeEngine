#pragma once
#include "BlazeEngine/DataStructures/Buffer.h"

namespace Blaze
{
	namespace OpenGL
	{				
		class BLAZE_API GraphicsBuffer
		{			
			uint id;
		public:
			GraphicsBuffer();
			GraphicsBuffer(GraphicsBuffer&&) noexcept;
			GraphicsBuffer(const GraphicsBuffer&) = delete;
			~GraphicsBuffer();						

			inline uint GetHandle() const { return id; }

			GraphicsBuffer& operator=(const GraphicsBuffer&) = delete;
			GraphicsBuffer& operator=(GraphicsBuffer&&) noexcept;			
		};
		
		enum class ImmutableGraphicsBufferMapAccess
		{			
			Read = 1,
			Write = 2,
		};
		enum class ImmutableGraphicsBufferMapType
		{
			None = 0,
			PersistentUncoherent = 1,
			PersistentCoherent = 2,
		};
		enum class ImmutableGraphicsBufferMapOptions
		{
			None = 0,
			InvalidateRange = 1,
			InvalidateBuffer = 2,
			ExplicitFlush = 4,
			Unsynchronized = 8
		};

		ENUM_CLASS_BITWISE_OPERATIONS(ImmutableGraphicsBufferMapAccess);
		ENUM_CLASS_BITWISE_OPERATIONS(ImmutableGraphicsBufferMapType);
		ENUM_CLASS_BITWISE_OPERATIONS(ImmutableGraphicsBufferMapOptions);


		class BLAZE_API ImmutableStaticGraphicsBuffer : public GraphicsBuffer
		{			 
		public:
			ImmutableStaticGraphicsBuffer() = default;			
			ImmutableStaticGraphicsBuffer(ImmutableStaticGraphicsBuffer&& other) noexcept : GraphicsBuffer(std::move(other)) { }			

			Result Allocate(void* ptr, size_t size, bool clientStorage = false);
			
			ImmutableStaticGraphicsBuffer& operator=(ImmutableStaticGraphicsBuffer&& other) noexcept { GraphicsBuffer::operator=(std::move(other)); return *this; }
		};

		class BLAZE_API ImmutableDynamicGraphicsBuffer : public GraphicsBuffer
		{
		public:
			ImmutableDynamicGraphicsBuffer() = default;
			ImmutableDynamicGraphicsBuffer(ImmutableDynamicGraphicsBuffer&& other) noexcept : GraphicsBuffer(std::move(other)) { }

			Result Allocate(void* ptr, size_t size, bool clientStorage = false);

			Result WriteData(const void* ptr, size_t size, size_t offset);

			ImmutableDynamicGraphicsBuffer& operator=(ImmutableDynamicGraphicsBuffer&& other) noexcept { GraphicsBuffer::operator=(std::move(other)); return *this; }
		};

		class BLAZE_API ImmutableMappedGraphicsBuffer : public GraphicsBuffer
		{
			ImmutableGraphicsBufferMapAccess mapAccess;
			ImmutableGraphicsBufferMapType mappingType;
		public:
			ImmutableMappedGraphicsBuffer();
			ImmutableMappedGraphicsBuffer(ImmutableMappedGraphicsBuffer&& other) noexcept;

			Result Allocate(void* ptr, size_t size, ImmutableGraphicsBufferMapAccess access, ImmutableGraphicsBufferMapType type, bool clientStorage = false);

			void* MapBufferRange(size_t offset, size_t size, ImmutableGraphicsBufferMapOptions options);
			Result UnmapBuffer();
			Result FlushBufferRange(size_t offset, size_t size);

			ImmutableMappedGraphicsBuffer& operator=(ImmutableMappedGraphicsBuffer&& other) noexcept;
		};
		
		enum class MutableGraphicsBufferUseFrequency
		{
			Static,
			Dynamic,
			Stream
		};		

		class BLAZE_API MutableDrawGraphicsBuffer : public GraphicsBuffer
		{
		public:
			MutableDrawGraphicsBuffer() = default;
			MutableDrawGraphicsBuffer(MutableDrawGraphicsBuffer&& other) noexcept : GraphicsBuffer(std::move(other)) { }

			Result Allocate(void* ptr, size_t size, MutableGraphicsBufferUseFrequency frequency);

			Result WriteData(const void* ptr, size_t size, size_t offset);

			MutableDrawGraphicsBuffer& operator=(MutableDrawGraphicsBuffer&& other) noexcept { GraphicsBuffer::operator=(std::move(other)); return *this; }
		};

		class BLAZE_API MutableReadGraphicsBuffer : public GraphicsBuffer
		{
		public:
			MutableReadGraphicsBuffer() = default;
			MutableReadGraphicsBuffer(MutableReadGraphicsBuffer&& other) noexcept : GraphicsBuffer(std::move(other)) { }

			Result Allocate(size_t size, MutableGraphicsBufferUseFrequency frequency);			

			Result ReadData(void* ptr, size_t size, size_t offset) const;

			MutableReadGraphicsBuffer& operator=(MutableReadGraphicsBuffer&& other) noexcept { GraphicsBuffer::operator=(std::move(other)); return *this; }
		};

		class BLAZE_API MutableCopyGraphicsBuffer : public GraphicsBuffer
		{
		public:
			MutableCopyGraphicsBuffer() = default;
			MutableCopyGraphicsBuffer(MutableCopyGraphicsBuffer&& other) noexcept : GraphicsBuffer(std::move(other)) { }

			Result Allocate(size_t size, MutableGraphicsBufferUseFrequency frequency);

			Result CopyData(GraphicsBuffer& destination, size_t readOffset, size_t writeOffset, size_t size);

			MutableCopyGraphicsBuffer& operator=(MutableCopyGraphicsBuffer&& other) noexcept { GraphicsBuffer::operator=(std::move(other)); return *this; }
		};
	}
}