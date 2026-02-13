#pragma once
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"

namespace Blaze::Graphics::OpenGL
{
	class GraphicsBuffer;

	class BLAZE_API VertexArray
	{
		uint id;
	public:
		VertexArray();
		//0 is an invalid value, so use that to denote a released/unallocated vertex array
		VertexArray(uint id);
		VertexArray(const VertexArray&) = delete;
		VertexArray(VertexArray&&) noexcept;
		~VertexArray();

		void SetIndexBuffer(const GraphicsBuffer& buffer);

		void EnableVertexAttribute(uint index);
		void DisableVertexAttribute(uint index);

		void SetIntegerVertexAttributeFormat(uint index, IntegerVertexAttributeType type, uintMem count, uintMem offset);
		void SetDoubleVertexAttributeFormat(uint index, uintMem count, uintMem offset);
		void SetFloatVertexAttributeFormat(uint index, FloatVertexAttributeType type, uintMem count, uintMem offset);
		void SetFloatVertexAttributeAsNormalizedFormat(uint index, IntegerVertexAttributeType type, uintMem count, uintMem offset);
		void SetFloatVertexAttributeAsPackedFormat(uint index, PackedVertexAttributeType type, uintMem count, uintMem offset);
		void SetFloatVertexAttributeAsBGRAFormat(uint index, BGRAVertexAttributeType type, uintMem count, uintMem offset);

		void SetVertexAttributeBuffer(uint index, const GraphicsBuffer* buffer, uintMem stride, uintMem offset);

		//If divisor is non-zero, the attributes advance once per divisor instances of the set(s) of vertices being rendered
		void SetVertexAttributeDivisor(uint index, uint divisor);

		void Swap(VertexArray& other);

		[[nodiscard]]
		uint ReleaseHandleOwnership();
		void Release();
		inline uint GetHandle() const { return id; }

		VertexArray& operator=(const VertexArray&) = delete;
		VertexArray& operator=(VertexArray&&) noexcept;
	};
}