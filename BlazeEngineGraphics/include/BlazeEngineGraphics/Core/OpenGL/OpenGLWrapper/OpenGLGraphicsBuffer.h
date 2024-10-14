#pragma once
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"

namespace Blaze::Graphics::OpenGLWrapper
{
	class BLAZE_CORE_API GraphicsBuffer
	{
		uint id;
	public:
		GraphicsBuffer();
		GraphicsBuffer(GraphicsBuffer&&) noexcept;
		GraphicsBuffer(const GraphicsBuffer&) = delete;
		~GraphicsBuffer();

		inline uint GetHandle() const { return id; }

		void Invalidate() const;

		GraphicsBuffer& operator=(const GraphicsBuffer&) = delete;
		GraphicsBuffer& operator=(GraphicsBuffer&&) noexcept;
	};

	class BLAZE_CORE_API ImmutableStaticGraphicsBuffer : public GraphicsBuffer
	{
	public:
		ImmutableStaticGraphicsBuffer() = default;
		ImmutableStaticGraphicsBuffer(ImmutableStaticGraphicsBuffer&& other) noexcept : GraphicsBuffer(std::move(other)) { }

		void Allocate(void* ptr, size_t size, bool clientStorage = false);

		ImmutableStaticGraphicsBuffer& operator=(ImmutableStaticGraphicsBuffer&& other) noexcept { GraphicsBuffer::operator=(std::move(other)); return *this; }
	};

	class BLAZE_CORE_API ImmutableDynamicGraphicsBuffer : public GraphicsBuffer
	{
	public:
		ImmutableDynamicGraphicsBuffer() = default;
		ImmutableDynamicGraphicsBuffer(ImmutableDynamicGraphicsBuffer&& other) noexcept : GraphicsBuffer(std::move(other)) { }

		void Allocate(void* ptr, size_t size, bool clientStorage = false);

		void WriteData(const void* ptr, size_t size, size_t offset);

		ImmutableDynamicGraphicsBuffer& operator=(ImmutableDynamicGraphicsBuffer&& other) noexcept { GraphicsBuffer::operator=(std::move(other)); return *this; }
	};

	class BLAZE_CORE_API ImmutableMappedGraphicsBuffer : public GraphicsBuffer
	{
		ImmutableGraphicsBufferMapAccess mapAccess;
		ImmutableGraphicsBufferMapType mappingType;
	public:
		ImmutableMappedGraphicsBuffer();
		ImmutableMappedGraphicsBuffer(ImmutableMappedGraphicsBuffer&& other) noexcept;

		void Allocate(void* ptr, size_t size, ImmutableGraphicsBufferMapAccess access, ImmutableGraphicsBufferMapType type, bool clientStorage = false);

		void* MapBufferRange(size_t offset, size_t size, ImmutableGraphicsBufferMapOptions options);
		void UnmapBuffer();
		void FlushBufferRange(size_t offset, size_t size);

		ImmutableMappedGraphicsBuffer& operator=(ImmutableMappedGraphicsBuffer&& other) noexcept;
	};

	class BLAZE_CORE_API MutableDrawGraphicsBuffer : public GraphicsBuffer
	{
	public:
		MutableDrawGraphicsBuffer() = default;
		MutableDrawGraphicsBuffer(MutableDrawGraphicsBuffer&& other) noexcept : GraphicsBuffer(std::move(other)) { }

		void Allocate(void* ptr, size_t size, MutableGraphicsBufferUseFrequency frequency);

		void WriteData(const void* ptr, size_t size, size_t offset);

		MutableDrawGraphicsBuffer& operator=(MutableDrawGraphicsBuffer&& other) noexcept { GraphicsBuffer::operator=(std::move(other)); return *this; }
	};

	class BLAZE_CORE_API MutableReadGraphicsBuffer : public GraphicsBuffer
	{
	public:
		MutableReadGraphicsBuffer() = default;
		MutableReadGraphicsBuffer(MutableReadGraphicsBuffer&& other) noexcept : GraphicsBuffer(std::move(other)) { }

		void Allocate(size_t size, MutableGraphicsBufferUseFrequency frequency);

		void ReadData(void* ptr, size_t size, size_t offset) const;

		MutableReadGraphicsBuffer& operator=(MutableReadGraphicsBuffer&& other) noexcept { GraphicsBuffer::operator=(std::move(other)); return *this; }
	};

	class BLAZE_CORE_API MutableCopyGraphicsBuffer : public GraphicsBuffer
	{
	public:
		MutableCopyGraphicsBuffer() = default;
		MutableCopyGraphicsBuffer(MutableCopyGraphicsBuffer&& other) noexcept : GraphicsBuffer(std::move(other)) { }

		void Allocate(size_t size, MutableGraphicsBufferUseFrequency frequency);

		void CopyData(GraphicsBuffer& destination, size_t readOffset, size_t writeOffset, size_t size);

		MutableCopyGraphicsBuffer& operator=(MutableCopyGraphicsBuffer&& other) noexcept { GraphicsBuffer::operator=(std::move(other)); return *this; }
	};
}