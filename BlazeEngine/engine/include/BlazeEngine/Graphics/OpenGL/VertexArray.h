#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Graphics/OpenGL/Buffer.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/Vector.h"
	
namespace Blaze
{
	namespace OpenGL
	{
		enum class AttributeType
		{
			Float,
			Vec2f,
			Vec3f,
			Vec4f,
		};

		template<AttributeType> struct GetTypeStruct { using type = InvalidType; };
		template<> struct GetTypeStruct<AttributeType::Float> { using type = float; };
		template<> struct GetTypeStruct<AttributeType::Vec2f> { using type = Vec2f; };
		template<> struct GetTypeStruct<AttributeType::Vec3f> { using type = Vec3f; };
		template<> struct GetTypeStruct<AttributeType::Vec4f> { using type = Vec4f; };
		template<AttributeType T> using GetType = GetTypeStruct<T>::template type;

		template<typename> struct GetAttributeTypeStruct { static constexpr AttributeType type = (AttributeType)0; };
		template<> struct GetAttributeTypeStruct<float> { static constexpr AttributeType type = AttributeType::Float; };
		template<> struct GetAttributeTypeStruct<Vec2f> { static constexpr AttributeType type = AttributeType::Vec2f; };
		template<> struct GetAttributeTypeStruct<Vec3f> { static constexpr AttributeType type = AttributeType::Vec3f; };
		template<> struct GetAttributeTypeStruct<Vec4f> { static constexpr AttributeType type = AttributeType::Vec4f; };
		template<typename T> static constexpr AttributeType GetAttributeType = GetAttributeTypeStruct<T>::template type;

		inline size_t SizeOf(AttributeType type)
		{
			switch (type)
			{
			case AttributeType::Float: return sizeof(GetType<AttributeType::Float>);
			case AttributeType::Vec2f: return sizeof(GetType<AttributeType::Vec2f>);
			case AttributeType::Vec3f: return sizeof(GetType<AttributeType::Vec3f>);
			case AttributeType::Vec4f: return sizeof(GetType<AttributeType::Vec4f>);			
			}
		}
		
		struct AttributeData
		{			
			const BaseBuffer* buffer = nullptr;
			size_t bufferStride = 0;
			size_t bufferOffset = 0;
			AttributeType type = (AttributeType)0;
			bool normalised = false;
			size_t attributeOffset = 0;

			AttributeData() = default;
			AttributeData(const BaseBuffer* buffer, size_t bufferStride, size_t bufferOffset, AttributeType type, bool normalised, size_t attributeOffset)
				: buffer(buffer), bufferStride(bufferStride), bufferOffset(bufferOffset), type(type), normalised(normalised), attributeOffset(attributeOffset)
			{

			}
		};

		class BLAZE_API VertexArray
		{
			uint id;
			BaseBuffer* arrayBuffer;
			BaseBuffer* indexBuffer;
		public:
			inline VertexArray();
			inline VertexArray(const VertexArray&) = delete;
			inline VertexArray(VertexArray&&) noexcept;
			inline ~VertexArray();
							
			inline BaseBuffer* GetIndexBuffer() const;
			inline void SetIndexBuffer(BaseBuffer& buffer);

			inline void EnableVertexAttribute(uint index);
			inline void DisableVertexAttribute(uint index);
			inline void VertexAttributeVertexBuffer(uint index, const BaseBuffer& buffer, size_t bufferStride, size_t bufferOffset);
			inline void VertexAttributeFormat(uint index, AttributeType type, bool normalised, size_t attributeOffset);

			inline void SetVertexAttributeData(uint index, const AttributeData& data)
			{
				EnableVertexAttribute(index);
				VertexAttributeVertexBuffer(index, *data.buffer, data.bufferStride, data.bufferOffset);
				VertexAttributeFormat(index, data.type, data.normalised, data.attributeOffset);
			}			

			static inline void Bind(const VertexArray& va);
			static inline void Unbind();
			static inline VertexArray* GetBound();

			inline VertexArray& operator=(const VertexArray&) = delete;
			inline VertexArray& operator=(VertexArray&&) noexcept;
			
			friend class BaseBuffer;
		};		
	}
}