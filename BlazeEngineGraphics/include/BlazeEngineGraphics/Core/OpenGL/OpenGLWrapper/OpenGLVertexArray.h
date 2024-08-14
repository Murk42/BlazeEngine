#pragma once
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"
	
namespace Blaze::Graphics::OpenGLWrapper
{	
	class GraphicsBuffer;

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
	public:
		VertexArray();
		VertexArray(const VertexArray&) = delete;
		VertexArray(VertexArray&&) noexcept;
		~VertexArray();
		
		void SetIndexBuffer(GraphicsBuffer& buffer);

		void EnableVertexAttribute(uint index);
		void DisableVertexAttribute(uint index);

		void SetVertexAttributeBuffer(uint index, const GraphicsBuffer* buffer, uint stride, uint offset);
		void SetVertexAttributeFormat(uint index, VertexAttributeType type, uint count, bool normalised, uint offset);
		void SetVertexAttributeDivisor(uint index, uint divisor);

		inline uint GetHandle() const { return id; }

		VertexArray& operator=(const VertexArray&) = delete;
		VertexArray& operator=(VertexArray&&) noexcept;

		friend class GraphicsBuffer;
	};
}