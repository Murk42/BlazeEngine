#pragma once

namespace Graphics2
{	
	class RenderWindow;

	class RenderContext
	{
	public:		
		RenderContext();
		virtual ~RenderContext();

		inline uint64 GetBackendID() const { return backendID; }

		template<typename T>
		static uint64 AddCustomBackend();
		template<AllocatorType Allocator = DefaultAllocator>
		static Handle<RenderContext, Allocator> New(uint64 backendID);
		template<AllocatorType Allocator = DefaultAllocator>
		static Handle<RenderContext, Allocator> NewOpenGLContext();
		template<AllocatorType Allocator = DefaultAllocator>
		static Handle<RenderContext, Allocator> NewVulkanContext();
	private:
		struct BackendCreateData
		{
			uint size;
			uint baseOffset;
			void(*constructor)(void*);
		};

		static List<BackendCreateData> createData;
		static uint64 lastBackendID;

		static uint64 GetOpenGLBackendID();
		static uint64 GetVulkanBackendID();

		template<typename T>
		static void Construct(void* ptr);

		uint64 backendID;
	};

	template<typename T>
	inline uint64 RenderContext::AddCustomBackend()
 	{
		BackendCreateData data;
		data.size = sizeof(T);
		data.constructor = &Construct<T>;
		data.baseOffset = BaseOffset<RenderContext, T>();
		return (uint64)createData.AddBack(data).Ptr();
	}

	template<AllocatorType Allocator>
	inline Handle<RenderContext, Allocator> RenderContext::New(uint64 backendID)
	{
		BackendCreateData* data = (BackendCreateData*)backendID;
		auto ptr = (RenderContext*)((byte*)Allocator::Allocate(data->size) + data->baseOffset);
		lastBackendID = backendID;
		data->constructor(ptr);
		return Handle<RenderContext, Allocator>(ptr);
	}

	template<AllocatorType Allocator>
	inline Handle<RenderContext, Allocator> RenderContext::NewOpenGLContext()
	{
		return New(GetOpenGLBackendID());
	}

	template<AllocatorType Allocator>
	inline Handle<RenderContext, Allocator> RenderContext::NewVulkanContext()
	{
		return New(GetVulkanBackendID());
	}
	template<typename T>
	inline void RenderContext::Construct(void* ptr)
	{
		std::construct_at<T>((T*)ptr);				
	}
}