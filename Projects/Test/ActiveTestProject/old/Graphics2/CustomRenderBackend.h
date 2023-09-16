#pragma once
#include "RenderContext.h"

namespace Graphics2
{

	template<typename T, typename C>
	concept ValidRenderBackend = requires {
		typename T::template ContextType;
	}&&
		std::constructible_from<typename T::template ContextType>&&
		std::constructible_from<T, typename T::template ContextType*>&&
		std::derived_from<T, C>;

	template<typename T>
	concept ValidRenderFrontend = true;

	template<ValidRenderFrontend T>
	class CustomRenderBackend
	{
	public:
		template<ValidRenderBackend<T> T_Backend>
		static void SetCustomBackend(uint64 renderContextBackend);
		template<ValidRenderBackend<T> T_Backend>
		static uint64 AddCustomBackend();		

		template<AllocatorType Allocator = DefaultAllocator>
		static Handle<T, Allocator> New(RenderContext*);
		template<AllocatorType Allocator = DefaultAllocator>
		static Handle<T, Allocator> New(RenderContext*, uint64 backendID);
		
	private:
		struct BackendCreateData
		{
			uint size;
			uint baseOffset;
			void(*constructor)(void*, void*);
		};

		template<ValidRenderBackend<T> T_Backend>
		static void Construct(void* ptr, void* context);

		inline static List<BackendCreateData> createDataList;
		inline static Map<uint64, BackendCreateData> createDataMap;
	};

	template<ValidRenderFrontend T>
	template<ValidRenderBackend<T> T_Backend>
	inline void CustomRenderBackend<T>::SetCustomBackend(uint64 renderContextBackend)
	{
		BackendCreateData data;
		data.size = sizeof(T_Backend);
		data.constructor = &Construct<T_Backend>;
		data.baseOffset = BaseOffset<T, T_Backend>();
		createDataMap.Insert(renderContextBackend, data);
	}

	template<ValidRenderFrontend T>
	template<ValidRenderBackend<T> T_Backend>
	inline uint64 CustomRenderBackend<T>::AddCustomBackend()
	{
		BackendCreateData data;
		data.size = sizeof(T_Backend);
		data.constructor = &Construct<T_Backend>;
		data.baseOffset = BaseOffset<T, T_Backend>();
		return (uint64)createDataList.AddBack(data).Ptr();
	}

	template<ValidRenderFrontend T>
	template<AllocatorType Allocator>
	inline Handle<T, Allocator> CustomRenderBackend<T>::New(RenderContext* renderContext)
	{
		BackendCreateData* data = &createDataMap.Find(renderContext->GetBackendID())->value;
		T* ptr = (T*)((byte*)Allocator::Allocate(data->size) + data->baseOffset);
		data->constructor(ptr, renderContext);
		return Handle<T, Allocator>(ptr);
	}

	template<ValidRenderFrontend T>
	template<AllocatorType Allocator>
	inline Handle<T, Allocator> CustomRenderBackend<T>::New(RenderContext*, uint64 backendID)
	{
		BackendCreateData* data = (BackendCreateData*)backendID;
		T* ptr = (T*)((byte*)Allocator::Allocate(data->size) + data->baseOffset);
		data->constructor(ptr);
		return Handle<T, Allocator>(ptr);
	}

	template<ValidRenderFrontend T>
	template<ValidRenderBackend<T> T_Backend>
	inline void CustomRenderBackend<T>::Construct(void* ptr, void* context)
	{
		using ContextType = typename T_Backend::template ContextType;
		std::construct_at<T_Backend, ContextType*>((T_Backend*)ptr, (ContextType*)(RenderContext*)context);
	}
}