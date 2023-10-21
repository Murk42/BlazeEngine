#pragma once
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

template<typename T, typename Base>
concept ImplementationOf = std::derived_from<T, Base>&& std::constructible_from<T, T&&>;

template<typename Derived, AllocatorType Allocator = DefaultAllocator>
class DynamicImplementation
{
public:
	DynamicImplementation(const DynamicImplementation&) = delete;
	DynamicImplementation(DynamicImplementation&& other) noexcept;
	//DynamicImplementation(Derived&& other) noexcept;
	//template<ImplementationOf<Derived> T>
	//DynamicImplementation(T&& other);
	virtual ~DynamicImplementation();

	inline bool IsImplemented() const { return implementation != nullptr; }

	template<ImplementationOf<Derived> T>
	inline T* TryCast();

	DynamicImplementation& operator=(const DynamicImplementation&) = delete;
	DynamicImplementation& operator=(DynamicImplementation&& other) noexcept;
	//template<ImplementationOf<Derived> T>
	//DynamicImplementation& operator=(T&& other);
protected:
	DynamicImplementation();

	template<ImplementationOf<Derived> T>
	inline void SetImplementation(T&& other);
	
	inline Derived* GetImplementation() const { return implementation; }
private:
	Derived* implementation;
};

namespace Graphics3
{
	enum class ImplementationAPI
	{
		Vulkan,
		OpenGL,

		Other
	};

	struct ImplementationInfo
	{
		ImplementationAPI api;
		String name;
	};
	
	ImplementationInfo* RegisterImplementation(const ImplementationInfo& info);
	ImplementationInfo* GetImplementationInfo(String name);

	namespace Dynamic
	{		
#define GRAPHICS_CLASS_DYNAMIC_CONSTRUCTOR(class_name) class_name(BaseType* base, ImplementationInfo* info) : DynamicWrapper(base, info) { }
		template<typename T>
		class DynamicWrapper
		{
		public:
			using BaseType = T;

			DynamicWrapper(T* base, ImplementationInfo* info);
			DynamicWrapper(DynamicWrapper&&) noexcept;
			~DynamicWrapper();

			void Clear();
			
			inline ImplementationInfo* GetImplementationInfo() const { return info; }
			inline T* GetPtr() const { return base; }

			DynamicWrapper& operator=(DynamicWrapper&& other) noexcept;
		protected:
			ImplementationInfo* info;
			T* base;
		};
		
		class GraphicsContextBase
		{
		public:
			GraphicsContextBase(const GraphicsContextBase&) = delete;
			GraphicsContextBase(GraphicsContextBase&& other) noexcept;
			GraphicsContextBase(ImplementationInfo* info);
			template<ImplementationOf<GraphicsContextBase> T>
			GraphicsContextBase(T&& other);
			virtual ~GraphicsContextBase();

			inline bool IsImplemented() const { return implementation != nullptr; }
			void Destroy();

			virtual void WaitForIdle() const;
			virtual String GetImplementationName() const;

			template<ImplementationOf<GraphicsContextBase> T>
			inline T* TryCast();

			GraphicsContextBase& operator=(const GraphicsContextBase&) = delete;
			GraphicsContextBase& operator=(GraphicsContextBase&& other) noexcept;
		protected:
			GraphicsContextBase();
		private:
			GraphicsContextBase* implementation;
		};		

		class SemaphoreBase
		{
		public:
			SemaphoreBase(const SemaphoreBase&) = delete;
			SemaphoreBase(SemaphoreBase&& other) noexcept;
			SemaphoreBase(GraphicsContextBase& graphicsContext);
			template<ImplementationOf<SemaphoreBase> T>
			SemaphoreBase(T&& other);
			virtual ~SemaphoreBase();

			inline bool IsImplemented() const { return implementation != nullptr; }
			void Destroy();

			SemaphoreBase& operator=(const SemaphoreBase&) = delete;
			SemaphoreBase& operator=(SemaphoreBase&& other) noexcept;
		protected:
			SemaphoreBase();
		private:
			SemaphoreBase* implementation;
		};		

		template<ImplementationOf<SemaphoreBase> T>
		SemaphoreBase::SemaphoreBase(T&& other)
			: implementation(DefaultAllocator::Create<T>(std::move(other)))
		{			
		}

		class RenderContextBase
		{
		public:
			virtual ~RenderContextBase();

			virtual void Clear() = 0;
			virtual void WaitForIdle() const = 0;
		};
		class RenderContext : public DynamicWrapper<RenderContextBase>
		{
		public:			
			GRAPHICS_CLASS_DYNAMIC_CONSTRUCTOR(RenderContext)

			inline void Clear() const { return base->Clear(); }
			inline void WaitForIdle() const { base->WaitForIdle(); }

			inline operator RenderContextBase& () { return *base; }
		};

		class FramebufferBase
		{
		public:
			virtual ~FramebufferBase();

			virtual void Clear() = 0;
			virtual Vec2i GetSize() const = 0;
		};
		class Framebuffer : public DynamicWrapper<FramebufferBase>
		{		
		public:
			GRAPHICS_CLASS_DYNAMIC_CONSTRUCTOR(Framebuffer)

			inline void Clear() const { return base->Clear(); }
			inline Vec2i GetSize() const { return base->GetSize(); }
		};		

		class RenderWindowBase;

		class RenderWindowFramebufferBase
		{
		public:			
			virtual ~RenderWindowFramebufferBase();			

			virtual void Clear() = 0;
			virtual Vec2i GetSize() const = 0;
			virtual operator FramebufferBase&() = 0;
			virtual RenderWindowBase* GetRenderWindow() const = 0;
		};

		class RenderWindowFramebuffer : public DynamicWrapper<RenderWindowFramebufferBase>
		{
		public:
			GRAPHICS_CLASS_DYNAMIC_CONSTRUCTOR(RenderWindowFramebuffer)

			inline void Clear() const { return base->Clear(); }
			inline Vec2i GetSize() const { return base->GetSize(); }
			inline RenderWindowBase* GetRenderWindow() const { return base->GetRenderWindow(); }
		};

		class RenderWindowBase
		{
		public:			
			RenderWindowBase(const RenderWindowBase&) = delete;
			RenderWindowBase(RenderWindowBase&& other) noexcept;
			RenderWindowBase(GraphicsContextBase& graphicsContext, String title, Vec2i pos, Vec2i size);
			template<ImplementationOf<RenderWindowBase> T>
			RenderWindowBase(T&& other);
			virtual ~RenderWindowBase();

			inline bool IsImplemented() const { return implementation != nullptr; }
			void Destroy();			

			virtual WindowBase* GetWindow() const;
			virtual Vec2i GetSize() const;

			virtual RenderWindowFramebufferBase& AcquireNextFramebuffer(SemaphoreBase& semaphore);

			template<ImplementationOf<RenderWindowBase> T>
			inline T* TryCast();			

			RenderWindowBase& operator=(const RenderWindowBase&) = delete;
			RenderWindowBase& operator=(RenderWindowBase&& other) noexcept;
		protected:
			RenderWindowBase();
		private:
			RenderWindowBase* implementation;
		};

		template<ImplementationOf<GraphicsContextBase> T>
		inline GraphicsContextBase::GraphicsContextBase(T&& other)
		{
			implementation = DefaultAllocator::Create<T>(std::move(other));
		}
		template<ImplementationOf<GraphicsContextBase> T>
		inline T* GraphicsContextBase::TryCast()
		{
			if (implementation == nullptr)
				return dynamic_cast<T*>(this);
			return dynamic_cast<T*>(implementation);
		}
		template<ImplementationOf<RenderWindowBase> T>
		RenderWindowBase::RenderWindowBase(T&& other)
		{
			implementation = DefaultAllocator::Create<T>(std::move(other));
		}
		template<ImplementationOf<RenderWindowBase> T>
		inline T* RenderWindowBase::TryCast()
		{
			if (implementation == nullptr)
				return dynamic_cast<T*>(this);
			return dynamic_cast<T*>(implementation);
		}		
		
		template<typename T>
		inline DynamicWrapper<T>::DynamicWrapper(T* base, ImplementationInfo* info)
			: base(base), info(info)
		{
		}
		template<typename T>
		inline DynamicWrapper<T>::DynamicWrapper(DynamicWrapper&& other) noexcept
		{
			base = other.base;
			info = other.info;

			other.base = nullptr;
			other.info = nullptr;
		}
		template<typename T>
		inline DynamicWrapper<T>::~DynamicWrapper()
		{
			Clear();
		}
		template<typename T>
		inline void DynamicWrapper<T>::Clear()
		{
			if (base != nullptr)
				delete base;

			base = nullptr;
			info = nullptr;
		}
		template<typename T>
		inline DynamicWrapper<T>& DynamicWrapper<T>::operator=(DynamicWrapper&& other) noexcept
		{
			Clear();

			base = other.base;
			info = other.info;

			other.base = nullptr;
			other.info = nullptr;

			return *this;
		}				
}
}

template<typename Derived, AllocatorType Allocator>
inline DynamicImplementation<Derived, Allocator>::DynamicImplementation(DynamicImplementation&& other) noexcept
	: implementation(other.implementation)
{
	other.implementation = nullptr;
}
//template<typename Derived, AllocatorType Allocator>
//inline DynamicImplementation<Derived, Allocator>::DynamicImplementation(Derived&& other) noexcept
//	: implementation(other.implementation)
//{
//	other.implementation = nullptr;
//}
//template<typename Derived, AllocatorType Allocator>
//template<ImplementationOf<Derived> T>
//inline DynamicImplementation<Derived, Allocator>::DynamicImplementation(T&& other)
//	: implementation(nullptr)
//{
//	implementation = Allocator::Create<T>(std::move(other));
//}
template<typename Derived, AllocatorType Allocator>
inline DynamicImplementation<Derived, Allocator>::~DynamicImplementation()
{
	Allocator::Destroy(implementation);
	implementation = nullptr;
}
template<typename Derived, AllocatorType Allocator>
template<ImplementationOf<Derived> T>
inline T* DynamicImplementation<Derived, Allocator>::TryCast()
{
	if (implementation == nullptr)
		return dynamic_cast<T*>(this);
	return dynamic_cast<T*>(implementation);
}
template<typename Derived, AllocatorType Allocator>
inline DynamicImplementation<Derived, Allocator>& DynamicImplementation<Derived, Allocator>::operator=(DynamicImplementation&& other) noexcept
{
	Allocator::Destroy(implementation);
	implementation = other.implementation;
	other.implementation = nullptr;
	return *this;
}
//template<typename Derived, AllocatorType Allocator>
//template<ImplementationOf<Derived> T>
//inline DynamicImplementation<Derived, Allocator>& DynamicImplementation<Derived, Allocator>::operator=(T&& other)
//{
//	SetImplementation(std::move(other));
//	return *this;
//}
template<typename Derived, AllocatorType Allocator>
inline DynamicImplementation<Derived, Allocator>::DynamicImplementation()
	: implementation(nullptr)
{
}
template<typename Derived, AllocatorType Allocator>
template<ImplementationOf<Derived> T>
inline void DynamicImplementation<Derived, Allocator>::SetImplementation(T&& other)
{
	if (implementation == nullptr)
	{
		implementation = Allocator::Create<T>(std::move(other));
	}
	else
	{
		T* impl_T = TryCast<T>();

		if (impl_T == nullptr)
		{
			Allocator::Destroy(implementation);
			implementation = Allocator::Create<T>(std::move(other));
		}
		else
		{
			*impl_T = std::move(other);
		}
	}
}

	

