#pragma once

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
			virtual ~GraphicsContextBase();

			virtual void Clear() = 0;
			virtual void WaitForIdle() const = 0;
		};
		class GraphicsContext : public DynamicWrapper<GraphicsContextBase>
		{
		public:					
			GRAPHICS_CLASS_DYNAMIC_CONSTRUCTOR(GraphicsContext)

			inline void Clear() const { return base->Clear(); }
			inline void WaitForIdle() const { base->WaitForIdle(); }

			inline operator GraphicsContextBase& () { return *base; }
		};

		class SemaphoreBase
		{
		public:
			virtual ~SemaphoreBase();

			virtual void Clear() = 0;
		};
		class Semaphore : public DynamicWrapper<SemaphoreBase>
		{				
		public:
			GRAPHICS_CLASS_DYNAMIC_CONSTRUCTOR(Semaphore)

			inline void Clear() const { return base->Clear(); }

			inline operator SemaphoreBase& () { return *base; }
		};

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
			virtual ~RenderWindowBase();
			virtual void Clear() = 0;

			virtual Window* GetWindow() const = 0;
			virtual Vec2i GetSize() const = 0;
		
		};

		class RenderWindow : public DynamicWrapper<RenderWindowBase>
		{					
		public:
			GRAPHICS_CLASS_DYNAMIC_CONSTRUCTOR(RenderWindow)

			inline void Clear() const { return base->Clear(); }

			inline Window* GetWindow() const { return base->GetWindow(); }
			inline Vec2i GetSize() const { return base->GetSize(); }
		};

		GraphicsContext CreateGraphicsContext(ImplementationInfo* implementationInfo);
		Semaphore CreateSemaphore(GraphicsContext& graphicsContext);
		RenderContext CreateGraphicsRenderContext(GraphicsContext& graphicsContext);
		RenderContext CreatePresentRenderContext(GraphicsContext& graphicsContext);
		RenderWindow CreateRenderWindow(GraphicsContext& graphicsContext, String title, Vec2i size);
		RenderWindowFramebufferBase& AquireNextFramebuffer(RenderWindow& renderWindow, Semaphore& signalSemaphore);
		void PresentRenderWindow(RenderContext& renderContext, RenderWindow& renderWindow, RenderWindowFramebufferBase& framebuffer, std::initializer_list<Semaphore*> waitSemaphores);

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