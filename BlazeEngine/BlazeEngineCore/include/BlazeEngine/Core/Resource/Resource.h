#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Debug/Logger.h"
#include <atomic>

namespace Blaze
{
	class BLAZE_API ResourceBase
	{
	public:
		ResourceBase();
		virtual ~ResourceBase();

		bool IsLoaded() const;
		void WaitToLoad() const;

		void IncreaseReferenceCount();
		void DecreaseReferenceCount();
		inline uintMem GetReferenceCount() const;
	protected:
		void MarkAsLoaded();

		virtual void DestructResource() = 0;
	private:
		uint32 referenceCount;
		std::atomic_flag loaded;
	};

	template<typename T>
	class BLAZE_API Resource : public ResourceBase
	{
	public:
		using Type = T;

		template<typename ... Args>
		void LoadResource(Args&& ... args) requires IsConstructibleFrom<T, Args...>;

		T& Get();
		const T& Get() const;

		T* operator->();
		const T* operator->() const;

		__declspec(noinline) static uint64 GetTypeId()
		{
			volatile auto file = __FILE__;
			volatile auto line = __LINE__;
			volatile auto name = typeid(T).name();
			return (uint64)&GetTypeId;
		}
	private:
		struct alignas(alignof(T)) ResourceMemory
		{
			byte bytes[sizeof(T)];
		};
		ResourceMemory resourceMemory;

		void DestructResource() override;
	};

	template<typename T>
	template<typename ...Args>
	inline void Resource<T>::LoadResource(Args&& ...args) requires IsConstructibleFrom<T, Args...>
	{
		if (IsLoaded())
		{
			BLAZE_LOG_ERROR("Trying to load an already loaded resource");
			return;
		}

		std::construct_at<T>((T*)&resourceMemory, std::forward<Args>(args)...);

		MarkAsLoaded();
	}
	template<typename T>
	inline T& Resource<T>::Get()
	{
		WaitToLoad();
		return *(T*)&resourceMemory;
	}
	template<typename T>
	inline const T& Resource<T>::Get() const
	{
		WaitToLoad();
		return *(T*)&resourceMemory;
	}
	template<typename T>
	inline T* Resource<T>::operator->()
	{
		WaitToLoad();
		return *(T*)&resourceMemory;
	}
	template<typename T>
	inline const T* Resource<T>::operator->() const
	{
		WaitToLoad();
		return *(T*)&resourceMemory;
	}
	template<typename T>
	inline void Resource<T>::DestructResource()
	{
		std::destroy_at<T>((T*)&resourceMemory);
	}
}