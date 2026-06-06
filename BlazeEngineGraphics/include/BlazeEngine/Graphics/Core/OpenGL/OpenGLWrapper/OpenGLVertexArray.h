#pragma once
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"
#include <BlazeEngine/Core/Common/Variant.h>
#include <BlazeEngine/Core/Type/VectorTraits.h>

namespace Blaze::Graphics::OpenGL
{
	class GraphicsBuffer;

	using VertexAttributeType = Variant<IntegerVertexAttributeType, DoubleVertexAttributeType, FloatVertexAttributeType, IntegerVertexAttributeType, PackedVertexAttributeType, BGRAVertexAttributeType>;

	struct VertexAttributeDescriptor
	{
		uint attributeIndex;
		uintMem count;
		uintMem offset;
		VertexAttributeType type;
		uint divisor = 0;
		uint bindingIndex = 0;

		uintMem arraySize = 1;
		uintMem elementSize = 0;

		template<typename T, typename C>
		constexpr VertexAttributeDescriptor(uint attributeIndex, T C::* ptr, uint divisor = 0, uint bindingIndex = 0);
	};

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

		void EnableVertexAttribute(uint attributeIndex);
		void DisableVertexAttribute(uint attributeIndex);

		void SetVertexAttribute(const VertexAttributeDescriptor& descriptor);
		void SetIntegerVertexAttributeFormat(uint attributeIndex, IntegerVertexAttributeType type, uintMem count, uintMem offset);
		void SetDoubleVertexAttributeFormat(uint attributeIndex, DoubleVertexAttributeType type, uintMem count, uintMem offset);
		void SetFloatVertexAttributeFormat(uint attributeIndex, FloatVertexAttributeType type, uintMem count, uintMem offset);
		void SetFloatVertexAttributeAsNormalizedFormat(uint attributeIndex, NormalizedVertexAttributeType type, uintMem count, uintMem offset);
		void SetFloatVertexAttributeAsPackedFormat(uint attributeIndex, PackedVertexAttributeType type, uintMem count, uintMem offset);
		void SetFloatVertexAttributeAsBGRAFormat(uint attributeIndex, BGRAVertexAttributeType type, uintMem count, uintMem offset);

		void SetVertexAttributeBinding(uint attributeIndex, uint bindingIndex);

		void SetBindingBuffer(uint bindingIndex, const GraphicsBuffer* buffer, uintMem stride, uintMem offset);

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

	template<typename T, typename C>
	constexpr VertexAttributeDescriptor::VertexAttributeDescriptor(uint attributeIndex, T C::* ptr, uint divisor, uint bindingIndex)
		: attributeIndex(attributeIndex), count(1), offset(reinterpret_cast<uintMem>(&(reinterpret_cast<const C volatile*>(nullptr)->*ptr))), type(), divisor(divisor), bindingIndex(bindingIndex)
	{
		
		auto GetType = []<typename T2>() -> VertexAttributeType
		{
			static_assert(OneOf < T2, uint8, int8, uint16, int16, uint32, int32, float, double>, "Invalid vertex attribute type");
			if constexpr (SameAs<T2, uint8>) return IntegerVertexAttributeType::Uint8;
			else if constexpr (SameAs<T2, int8>) return IntegerVertexAttributeType::Int8;
			else if constexpr (SameAs<T2, uint16>) return IntegerVertexAttributeType::Uint16;
			else if constexpr (SameAs<T2, int16>) return IntegerVertexAttributeType::Int16;
			else if constexpr (SameAs<T2, uint32>) return IntegerVertexAttributeType::Uint32;
			else if constexpr (SameAs<T2, int32>) return IntegerVertexAttributeType::Int32;
			else if constexpr (SameAs<T2, float>) return FloatVertexAttributeType::Float;
			else if constexpr (SameAs<T2, double>) return DoubleVertexAttributeType::Double;
		};

		auto SetElement = [&]<typename T2>()
		{
			if constexpr (VectorType<T2>)
			{
				count = T2::Size;
				type = GetType.operator() < typename T2::template Type > ();
			}
			else
			{
				count = 1;
				type = GetType.operator() < T2 > ();
			}

			elementSize = sizeof(T2);
		};

		if constexpr (IsArrayType<T>)
		{
			arraySize = sizeof(T) / sizeof(ArrayValueType<T>);
			SetElement.operator() < ArrayValueType<T> > ();
		}	
		else
		{
			arraySize = 1;
			SetElement.operator()<T> ();
		}
	}
}