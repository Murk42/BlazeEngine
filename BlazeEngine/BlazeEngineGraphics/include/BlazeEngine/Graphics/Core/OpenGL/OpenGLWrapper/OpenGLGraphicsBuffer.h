#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"

namespace Blaze::Graphics::OpenGL
{
	class BLAZE_API GraphicsBuffer
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

	class BLAZE_API ImmutableStaticGraphicsBuffer : public GraphicsBuffer
	{
	public:
		ImmutableStaticGraphicsBuffer() = default;
		ImmutableStaticGraphicsBuffer(uint id) : GraphicsBuffer(id) { }
		ImmutableStaticGraphicsBuffer(ImmutableStaticGraphicsBuffer&& other) noexcept : GraphicsBuffer(std::move(other)) { }

		void Allocate(const void* ptr, uintMem size, bool clientStorage = false);

		void Swap(ImmutableStaticGraphicsBuffer& other);

		ImmutableStaticGraphicsBuffer& operator=(ImmutableStaticGraphicsBuffer&& other) noexcept { GraphicsBuffer::operator=(std::move(other)); return *this; }
	};

	class BLAZE_API ImmutableDynamicGraphicsBuffer : public GraphicsBuffer
	{
	public:
		ImmutableDynamicGraphicsBuffer() = default;
		ImmutableDynamicGraphicsBuffer(uint id) : GraphicsBuffer(id) { }
		ImmutableDynamicGraphicsBuffer(ImmutableDynamicGraphicsBuffer&& other) noexcept : GraphicsBuffer(std::move(other)) { }

		void Allocate(const void* ptr, uintMem size, bool clientStorage = false);
		void WriteData(const void* ptr, uintMem size, uintMem offset);

		void Swap(ImmutableDynamicGraphicsBuffer& other);

		ImmutableDynamicGraphicsBuffer& operator=(ImmutableDynamicGraphicsBuffer&& other) noexcept { GraphicsBuffer::operator=(std::move(other)); return *this; }
	};

	class BLAZE_API ImmutableMappedGraphicsBuffer : public GraphicsBuffer
	{
		GraphicsBufferMapAccessFlags mapAccess;
		GraphicsBufferMapType mappingType;
	public:
		ImmutableMappedGraphicsBuffer();
		ImmutableMappedGraphicsBuffer(uint id) : GraphicsBuffer(id) { }
		ImmutableMappedGraphicsBuffer(ImmutableMappedGraphicsBuffer&& other) noexcept;

		void Allocate(const void* ptr, uintMem size, GraphicsBufferMapAccessFlags access, GraphicsBufferMapType type, bool clientStorage = false);

		void* MapBufferRange(uintMem offset, uintMem size, GraphicsBufferMapOptions options);
		void UnmapBuffer();
		void FlushBufferRange(uintMem offset, uintMem size);

		void Swap(ImmutableMappedGraphicsBuffer& other);

		inline GraphicsBufferMapAccessFlags GetMapAccessOptions() const { return mapAccess; }
		inline GraphicsBufferMapType GetMapType() const { return mappingType; }

		ImmutableMappedGraphicsBuffer& operator=(ImmutableMappedGraphicsBuffer&& other) noexcept;
	};

	class BLAZE_API MutableDrawGraphicsBuffer : public GraphicsBuffer
	{
	public:
		MutableDrawGraphicsBuffer() = default;
		MutableDrawGraphicsBuffer(uint id) : GraphicsBuffer(id) { }
		MutableDrawGraphicsBuffer(MutableDrawGraphicsBuffer&& other) noexcept : GraphicsBuffer(std::move(other)) { }

		void Allocate(const void* ptr, uintMem size, GraphicsBufferUseFrequency frequency);
		void WriteData(const void* ptr, uintMem size, uintMem offset);

		void Swap(MutableDrawGraphicsBuffer& other);

		MutableDrawGraphicsBuffer& operator=(MutableDrawGraphicsBuffer&& other) noexcept { GraphicsBuffer::operator=(std::move(other)); return *this; }
	};

	class BLAZE_API MutableReadGraphicsBuffer : public GraphicsBuffer
	{
	public:
		MutableReadGraphicsBuffer() = default;
		MutableReadGraphicsBuffer(uint id) : GraphicsBuffer(id) { }
		MutableReadGraphicsBuffer(MutableReadGraphicsBuffer&& other) noexcept : GraphicsBuffer(std::move(other)) { }

		void Allocate(uintMem size, GraphicsBufferUseFrequency frequency);
		void ReadData(void* ptr, uintMem size, uintMem offset) const;

		void Swap(MutableReadGraphicsBuffer& other);

		MutableReadGraphicsBuffer& operator=(MutableReadGraphicsBuffer&& other) noexcept { GraphicsBuffer::operator=(std::move(other)); return *this; }
	};

	class BLAZE_API MutableCopyGraphicsBuffer : public GraphicsBuffer
	{
	public:
		MutableCopyGraphicsBuffer() = default;
		MutableCopyGraphicsBuffer(uint id) : GraphicsBuffer(id) { }
		MutableCopyGraphicsBuffer(MutableCopyGraphicsBuffer&& other) noexcept : GraphicsBuffer(std::move(other)) { }

		void Allocate(uintMem size, GraphicsBufferUseFrequency frequency);
		void CopyData(GraphicsBuffer& destination, uintMem readOffset, uintMem writeOffset, uintMem size);

		void Swap(MutableCopyGraphicsBuffer& other);

		MutableCopyGraphicsBuffer& operator=(MutableCopyGraphicsBuffer&& other) noexcept { GraphicsBuffer::operator=(std::move(other)); return *this; }
	};
}