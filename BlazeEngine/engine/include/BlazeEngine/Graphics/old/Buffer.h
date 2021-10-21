#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Types.h"
#include "BlazeEngine/DataStructures/Common.h"

namespace Blaze
{
	enum class GraphicsBufferType
	{		
		ArrayBuffer = 0x8892,
		IndexBuffer = 0x8893,
		UniformBuffer = 0x8A11
	};

	enum class BufferUsage
	{
		Stream = 0x88E0,
		Static = 0x88E4,
		Dynamic = 0x88E8,
		Draw = 0x0000,
		Read = 0x0001,
		Copy = 0x0002,
	};
	BufferUsage BLAZE_API operator| (const BufferUsage& a, const BufferUsage& b);

	class BLAZE_API Buffer
	{
		uint id;
		GraphicsBufferType type;
		BufferUsage usage;
		size_t size;						
	public:
		Buffer(GraphicsBufferType type);
		Buffer(const Buffer&);
		Buffer(Buffer&&) noexcept;
		Buffer(GraphicsBufferType type, const void* ptr, unsigned size, BufferUsage usage);
		~Buffer();

		/*
		Allocates new memory and deletes old if any. Size is measured in bytes.
		*/
		void AllocateData(const void* ptr, unsigned size, BufferUsage usage);
		/*
		Changes the existing memory block. Size and offset are measured in bytes.
		*/
		void ChangeData(const void* ptr, unsigned size, size_t offset);

		inline GraphicsBufferType GetType() const { return type; }
		inline size_t GetSize() const { return size; }
		inline BufferUsage GetUsage() const { return usage; }

		void Bind() const;
		static void Unbind(GraphicsBufferType type);
		static Buffer* GetBound(GraphicsBufferType type);

		Buffer& operator=(const Buffer&);
		Buffer& operator=(Buffer&&) noexcept;

		friend class ShaderProgram;
		friend class VertexLayout;
	};

	class BLAZE_API VertexBuffer : public Buffer
	{		
	public:
		VertexBuffer();
		VertexBuffer(const VertexBuffer&);
		VertexBuffer(VertexBuffer&&) noexcept;
		VertexBuffer(const void* ptr, unsigned size, BufferUsage usage);		

		VertexBuffer& operator=(const VertexBuffer&);
		VertexBuffer& operator=(VertexBuffer&&) noexcept;
	};

	class BLAZE_API IndexBuffer : public Buffer
	{
		Type indexType;
	public:			
		IndexBuffer();
		IndexBuffer(const IndexBuffer&);
		IndexBuffer(IndexBuffer&&) noexcept;
		IndexBuffer(const void* ptr, unsigned count, BufferUsage usage, Type indexType);

		/*
		Allocates new memory and deletes old if any. Size is measured in bytes.
		*/
		void AllocateData(const void* ptr, unsigned size, BufferUsage usage, Type indexType);		
		
		inline Type GetIndexType() const { return indexType; }
		inline uint GetIndexCount() const { return uint(GetSize() / SizeOf(indexType)); }

		IndexBuffer& operator=(const IndexBuffer&);
		IndexBuffer& operator=(IndexBuffer&&) noexcept;
		
	};
}