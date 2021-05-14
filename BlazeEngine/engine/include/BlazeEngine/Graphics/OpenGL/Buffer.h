#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"

namespace Blaze
{
	namespace OpenGL
	{		
		enum class BufferType
		{
			ArrayBuffer = 34962,
			IndexBuffer = 34963,
			UniformBuffer = 35345,
		};

		enum class BufferUsageHint
		{
			StreamDraw = 35040,
			StreamRead = 35041,
			StreamCopy = 35042,
			StaticDraw = 35044,
			StaticRead = 35045,
			StaticCopy = 35046,
			DynamicDraw = 35048,
			DynamicRead = 35049,
			DynamicCopy = 35050,
		};

		class BLAZE_API BaseBuffer
		{		
			size_t size;
			uint id;
		public:
			inline BaseBuffer();
			inline BaseBuffer(const BaseBuffer&) = delete;
			inline BaseBuffer(BaseBuffer&&) noexcept;
			inline ~BaseBuffer();			

			inline size_t GetSize() const;

			static inline void Bind(BufferType type, BaseBuffer& buffer);
			static inline void Unbind(BufferType type);
			static inline BaseBuffer* GetBound(BufferType type);

			inline BaseBuffer& operator=(const BaseBuffer&) = delete;
			inline BaseBuffer& operator=(BaseBuffer&&) noexcept;

			friend class VertexArray;
			friend class MutableBuffer;
			template<bool Static, bool Mapped>
			friend class ImmutableBuffer;
		};

		class BLAZE_API MutableBuffer : public BaseBuffer
		{			
		public:					
			inline void AllocateData(size_t size, const void* ptr, BufferUsageHint usage);			
			inline void ChangeData(size_t size, const void* ptr, size_t offset);
		};
		

		template<bool Static, bool Mapped = false>
		class ImmutableBuffer;
		
		template<>
		class BLAZE_API ImmutableBuffer<false, false> : public BaseBuffer
		{
		public:			
			inline void AllocateData(size_t size, const void* ptr);
			inline void ChangeData(size_t size, const void* ptr, size_t offset);
		};

		/*
		template<>
		class BLAZE_API ImmutableBuffer<false, true> : public BaseBuffer
		{
		public:
			inline void AllocateData(size_t size, const void* ptr);
		};
		*/

		template<>
		class BLAZE_API ImmutableBuffer<true, false> : public BaseBuffer
		{
		public:			
			inline void AllocateData(size_t size, const void* ptr);			
		};

		/*
		template<>
		class BLAZE_API ImmutableBuffer<true, true> : public BaseBuffer
		{
		public:
			inline void AllocateData(size_t size, const void* ptr);
		};
		*/
	}
}