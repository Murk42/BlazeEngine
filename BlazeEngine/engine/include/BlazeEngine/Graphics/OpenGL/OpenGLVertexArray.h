#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Graphics/OpenGL/OpenGLGraphicsBuffer.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/Vector.h"
	
namespace Blaze
{
	namespace OpenGL
	{
		enum class VertexAttributeType
		{
			Int8 = 0x1400,
			Uint8 = 0x1401,
			Int16 = 0x1402,
			Uint16 = 0x1403,
			Int32 = 0x1404,
			Uint32 = 0x1405,
			Float = 0x1406
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

		class BLAZE_API VertexArray
		{
			static constexpr size_t maxVertexAttributes = 8;
			uint id; 			
			GraphicsBuffer* indexBuffer;
			VertexAttribute attributes[maxVertexAttributes];
		public:
			VertexArray();
			VertexArray(const VertexArray&) = delete;
			VertexArray(VertexArray&&) noexcept;
			~VertexArray();
					
			GraphicsBuffer* GetIndexBuffer() const;			
			void SetIndexBuffer(GraphicsBuffer& buffer);

			void EnableVertexAttribute(uint index);
			void DisableVertexAttribute(uint index);			

			void SetVertexAttributeBuffer(uint index, const GraphicsBuffer* buffer, size_t stride, size_t offset);
			void SetVertexAttributeFormat(uint index, VertexAttributeType type, size_t count, bool normalised, size_t offset);
			void SetVertexAttributeData(uint index, const VertexAttribute& data);

			inline uint GetHandle() const { return id; }

			VertexArray& operator=(const VertexArray&) = delete;
			VertexArray& operator=(VertexArray&&) noexcept;
			
			friend class GraphicsBuffer;
		};		
	}
}