#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Types.h"
#include "BlazeEngine/DataStructures/Common.h"

namespace Blaze
{
	enum class BLAZE_API BufferType
	{		
		ArrayBuffer = 0x8892,
		IndexBuffer = 0x8893,
		UniformBuffer = 0x8A11
	};

	enum BLAZE_API BufferUsageType
	{
		Stream = 0x88E0,
		Static = 0x88E4,
		Dynamic = 0x88E8,
		Draw = 0x0000,
		Read = 0x0001,
		Copy = 0x0002,
	};
	BufferUsageType BLAZE_API operator| (const BufferUsageType& a, const BufferUsageType& b);

	class BLAZE_API Buffer
	{
		uint id;
		BufferType type;
		BufferUsageType usage;
		uint size;				
		
		static Buffer* boundUniformBuffer;
	public:
		Buffer(BufferType type);
		Buffer(const Buffer&);
		Buffer(Buffer&&);
		Buffer(BufferType type, const void* ptr, unsigned size, BufferUsageType usage);
		~Buffer();

		/*
		Allocates new memory and deletes old if any. Size is measured in bytes.
		*/
		void AllocateData(const void* ptr, unsigned size, BufferUsageType usage);
		/*
		Changes the existing memory block. Size and offset are measured in bytes.
		*/
		void ChangeData(const void* ptr, unsigned size, unsigned offset);

		inline BufferType GetType() const { return type; }
		inline unsigned GetSize() const { return size; }
		inline BufferUsageType GetUsage() const { return usage; }

		void Bind() const;
		static void Unbind(BufferType type);
		static Buffer* GetBound(BufferType type);

		void operator=(const Buffer&);
		void operator=(Buffer&&);

		friend class ShaderProgram;
		friend class VertexLayout;
	};

	class BLAZE_API VertexBuffer : public Buffer
	{		
	public:
		VertexBuffer();
		VertexBuffer(const VertexBuffer&);
		VertexBuffer(VertexBuffer&&);
		VertexBuffer(const void* ptr, unsigned size, BufferUsageType usage);		

		void operator=(const VertexBuffer&);
		void operator=(VertexBuffer&&);
	};

	class BLAZE_API IndexBuffer : public Buffer
	{
		Type indexType;
	public:			
		IndexBuffer();
		IndexBuffer(const IndexBuffer&);
		IndexBuffer(IndexBuffer&&);
		IndexBuffer(const void* ptr, unsigned count, BufferUsageType usage, Type indexType);

		/*
		Allocates new memory and deletes old if any. Size is measured in bytes.
		*/
		void AllocateData(const void* ptr, unsigned size, BufferUsageType usage, Type indexType);		
		
		inline Type GetIndexType() const { return indexType; }
		inline unsigned GetIndexCount() const { return GetSize() / SizeOf(indexType); }

		void operator=(const IndexBuffer&);
		void operator=(IndexBuffer&&);
		
	};
}