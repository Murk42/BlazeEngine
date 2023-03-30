#pragma once

namespace Blaze
{
	class BufferView;

	class BLAZE_API Buffer
	{
		void* ptr;
		size_t size;		
	public:
		Buffer();
		Buffer(const Buffer&);
		Buffer(Buffer&&) noexcept;
		Buffer(size_t size);
		Buffer(const BufferView&);
		~Buffer();

		inline size_t Size() const { return size; }
		inline void* Ptr() { return ptr; }
		inline const void* Ptr() const { return ptr; }

		void Allocate(size_t size);
		void Free();

		Buffer& operator=(const Buffer&);
		Buffer& operator=(Buffer&&) noexcept;
		Buffer& operator=(const BufferView&);
	};

	class BLAZE_API BufferView
	{
		const void* ptr;
		size_t size;
	public: 
		BufferView();
		BufferView(const BufferView&);		
		BufferView(const void* ptr, size_t size);
		BufferView(const Buffer&);

		inline size_t Size() const { return size; }
		inline const void* Ptr() const { return ptr; }

		BufferView& operator=(const BufferView&);
		BufferView& operator=(const Buffer&);
	};
}