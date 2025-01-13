#pragma once
#include "BlazeEngineCore/BlazeEngineCore.h"
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"

namespace Blaze::Graphics::OpenGLWrapper
{
	class BLAZE_GRAPHICS_API GraphicsBuffer
	{	
	public:
		GraphicsBuffer();
		GraphicsBuffer(uint id);
		GraphicsBuffer(GraphicsBuffer&&) noexcept;
		GraphicsBuffer(const GraphicsBuffer&) = delete;
		~GraphicsBuffer();

		void Invalidate() const;	

		uint ReleaseHandleOwnership();
		void Release();
		inline uint GetHandle() const { return id; }

		GraphicsBuffer& operator=(const GraphicsBuffer&) = delete;
		GraphicsBuffer& operator=(GraphicsBuffer&&) noexcept;
	protected:
		uint id;
	};

	class BLAZE_GRAPHICS_API ImmutableStaticGraphicsBuffer : public GraphicsBuffer
	{
	public:
		ImmutableStaticGraphicsBuffer() = default;
		ImmutableStaticGraphicsBuffer(uint id) : GraphicsBuffer(id) { }
		ImmutableStaticGraphicsBuffer(ImmutableStaticGraphicsBuffer&& other) noexcept : GraphicsBuffer(std::move(other)) { }

		void Allocate(const void* ptr, size_t size, bool clientStorage = false);

		void Swap(ImmutableStaticGraphicsBuffer& other);

		ImmutableStaticGraphicsBuffer& operator=(ImmutableStaticGraphicsBuffer&& other) noexcept { GraphicsBuffer::operator=(std::move(other)); return *this; }
	};

	class BLAZE_GRAPHICS_API ImmutableDynamicGraphicsBuffer : public GraphicsBuffer
	{
	public:
		ImmutableDynamicGraphicsBuffer() = default;
		ImmutableDynamicGraphicsBuffer(uint id) : GraphicsBuffer(id) { }
		ImmutableDynamicGraphicsBuffer(ImmutableDynamicGraphicsBuffer&& other) noexcept : GraphicsBuffer(std::move(other)) { }

		void Allocate(const void* ptr, size_t size, bool clientStorage = false);
		void WriteData(const void* ptr, size_t size, size_t offset);

		void Swap(ImmutableDynamicGraphicsBuffer& other);

		ImmutableDynamicGraphicsBuffer& operator=(ImmutableDynamicGraphicsBuffer&& other) noexcept { GraphicsBuffer::operator=(std::move(other)); return *this; }
	};

	class BLAZE_GRAPHICS_API ImmutableMappedGraphicsBuffer : public GraphicsBuffer
	{
		ImmutableGraphicsBufferMapAccess mapAccess;
		ImmutableGraphicsBufferMapType mappingType;
	public:
		ImmutableMappedGraphicsBuffer();
		ImmutableMappedGraphicsBuffer(uint id) : GraphicsBuffer(id) { }
		ImmutableMappedGraphicsBuffer(ImmutableMappedGraphicsBuffer&& other) noexcept;

		void Allocate(const void* ptr, size_t size, ImmutableGraphicsBufferMapAccess access, ImmutableGraphicsBufferMapType type, bool clientStorage = false);

		void* MapBufferRange(size_t offset, size_t size, ImmutableGraphicsBufferMapOptions options);
		void UnmapBuffer();
		void FlushBufferRange(size_t offset, size_t size);

		void Swap(ImmutableMappedGraphicsBuffer& other);

		ImmutableMappedGraphicsBuffer& operator=(ImmutableMappedGraphicsBuffer&& other) noexcept;
	};

	class BLAZE_GRAPHICS_API MutableDrawGraphicsBuffer : public GraphicsBuffer
	{
	public:
		MutableDrawGraphicsBuffer() = default;
		MutableDrawGraphicsBuffer(uint id) : GraphicsBuffer(id) { }
		MutableDrawGraphicsBuffer(MutableDrawGraphicsBuffer&& other) noexcept : GraphicsBuffer(std::move(other)) { }

		void Allocate(const void* ptr, size_t size, MutableGraphicsBufferUseFrequency frequency);
		void WriteData(const void* ptr, size_t size, size_t offset);

		void Swap(MutableDrawGraphicsBuffer& other);

		MutableDrawGraphicsBuffer& operator=(MutableDrawGraphicsBuffer&& other) noexcept { GraphicsBuffer::operator=(std::move(other)); return *this; }
	};

	class BLAZE_GRAPHICS_API MutableReadGraphicsBuffer : public GraphicsBuffer
	{
	public:
		MutableReadGraphicsBuffer() = default;
		MutableReadGraphicsBuffer(uint id) : GraphicsBuffer(id) { }
		MutableReadGraphicsBuffer(MutableReadGraphicsBuffer&& other) noexcept : GraphicsBuffer(std::move(other)) { }

		void Allocate(size_t size, MutableGraphicsBufferUseFrequency frequency);
		void ReadData(void* ptr, size_t size, size_t offset) const;

		void Swap(MutableReadGraphicsBuffer& other);

		MutableReadGraphicsBuffer& operator=(MutableReadGraphicsBuffer&& other) noexcept { GraphicsBuffer::operator=(std::move(other)); return *this; }
	};

	class BLAZE_GRAPHICS_API MutableCopyGraphicsBuffer : public GraphicsBuffer
	{
	public:
		MutableCopyGraphicsBuffer() = default;
		MutableCopyGraphicsBuffer(uint id) : GraphicsBuffer(id) { }
		MutableCopyGraphicsBuffer(MutableCopyGraphicsBuffer&& other) noexcept : GraphicsBuffer(std::move(other)) { }

		void Allocate(size_t size, MutableGraphicsBufferUseFrequency frequency);
		void CopyData(GraphicsBuffer& destination, size_t readOffset, size_t writeOffset, size_t size);

		void Swap(MutableCopyGraphicsBuffer& other);

		MutableCopyGraphicsBuffer& operator=(MutableCopyGraphicsBuffer&& other) noexcept { GraphicsBuffer::operator=(std::move(other)); return *this; }
	};
}