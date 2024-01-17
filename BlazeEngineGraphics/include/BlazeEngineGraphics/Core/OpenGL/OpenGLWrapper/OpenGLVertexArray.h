#pragma once
#include "OpenGLGraphicsBuffer.h"
	
namespace Blaze::Graphics::OpenGLWrapper
{
	enum class VertexAttributeType
	{
		Int8,
		Uint8,
		Int16,
		Uint16,
		Int32,
		Uint32,
		Float,
		Double
	};

	struct VertexAttribute
	{
		const GraphicsBuffer* buffer = nullptr;
		size_t bufferStride = 0;
		size_t bufferOffset = 0;
		VertexAttributeType type = (VertexAttributeType)0;
		size_t count = 0;
		bool normalised = false;
		size_t attributeOffset = 0;

		VertexAttribute() = default;
		VertexAttribute(const GraphicsBuffer* buffer, size_t bufferStride, size_t bufferOffset, VertexAttributeType type, size_t count, bool normalised, size_t attributeOffset)
			: buffer(buffer), bufferStride(bufferStride), bufferOffset(bufferOffset), type(type), count(count), normalised(normalised), attributeOffset(attributeOffset)
		{

		}
	};

	class BLAZE_GRAPHICS_API VertexArray
	{
		uint id;
		GraphicsBuffer* indexBuffer;
	public:
		VertexArray();
		VertexArray(const VertexArray&) = delete;
		VertexArray(VertexArray&&) noexcept;
		~VertexArray();

		GraphicsBuffer* GetIndexBuffer() const;
		Result SetIndexBuffer(GraphicsBuffer& buffer);

		Result EnableVertexAttribute(uint index);
		Result DisableVertexAttribute(uint index);

		Result SetVertexAttributeBuffer(uint index, const GraphicsBuffer* buffer, uint stride, uint offset);
		Result SetVertexAttributeFormat(uint index, VertexAttributeType type, uint count, bool normalised, uint offset);
		Result SetVertexAttributeDivisor(uint index, uint divisor);

		inline uint GetHandle() const { return id; }

		VertexArray& operator=(const VertexArray&) = delete;
		VertexArray& operator=(VertexArray&&) noexcept;

		friend class GraphicsBuffer;
	};
}