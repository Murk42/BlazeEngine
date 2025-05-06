#pragma once
#include "BlazeEngineCore/BlazeEngineCore.h"
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"
	
namespace Blaze::Graphics::OpenGLWrapper
{	
	class GraphicsBuffer;

	class BLAZE_GRAPHICS_API VertexArray
	{
		uint id;		
	public:
		VertexArray();
		VertexArray(uint id);
		VertexArray(const VertexArray&) = delete;
		VertexArray(VertexArray&&) noexcept;
		~VertexArray();

		
		void SetIndexBuffer(GraphicsBuffer& buffer);

		void EnableVertexAttribute(uint index);
		void DisableVertexAttribute(uint index);

		void SetVertexAttributeBuffer(uint index, const GraphicsBuffer* buffer, uintMem stride, uintMem offset);
		void SetVertexAttributeFormat(uint index, VertexAttributeType type, uintMem count, bool normalised, uintMem offset);
		void SetVertexAttributeDivisor(uint index, uint divisor);

		void Swap(VertexArray& other);

		uint ReleaseHandleOwnership();
		void Release();
		inline uint GetHandle() const { return id; }

		VertexArray& operator=(const VertexArray&) = delete;
		VertexArray& operator=(VertexArray&&) noexcept;

		friend class GraphicsBuffer;
	};
}