#include "pch.h"
#include "API_dynamic.h"
#include "../OpenGL/API_OpenGL.h" 
#include "../Vulkan/API_Vulkan.h"

namespace Graphics3
{
    List<ImplementationInfo> implementations;
    Map<String, ImplementationInfo*> implementationMap;

    ImplementationInfo* Graphics3::RegisterImplementation(const ImplementationInfo& info)
    {
        auto& ptr = *implementations.AddBack(info);
        implementationMap.Insert(info.name, &ptr);
        return &ptr;
    }
    ImplementationInfo* Graphics3::GetImplementationInfo(String name)
    {
        return implementationMap.Find(name)->value;
    }
}
namespace Graphics3::Dynamic
{    

    GraphicsContextBase::GraphicsContextBase(GraphicsContextBase&& other) noexcept
        : implementation(other.implementation)
    {
        other.implementation = nullptr;
    }
    GraphicsContextBase::GraphicsContextBase(ImplementationInfo* info)
    {
        switch (info->api)
        {
        case ImplementationAPI::OpenGL:
            implementation = DefaultAllocator::Create<OpenGL::GraphicsContext_OpenGL>();
            break;
        case ImplementationAPI::Vulkan:
        case ImplementationAPI::Other:
            Debug::Logger::LogError("Blaze graphics API", "Unsupported graphics API.");
            break;
        default:
            Debug::Logger::LogError("Blaze graphics API", "Invalid implementation API value.");
            break;
        }
    }
    GraphicsContextBase::~GraphicsContextBase()
    {
        Destroy();
    }
    void GraphicsContextBase::Destroy()
    {
        if (implementation != nullptr)
        {
            DefaultAllocator::Destroy(implementation);
            implementation = nullptr;
        }
    }
    void GraphicsContextBase::WaitForIdle() const
    {
        if (implementation != nullptr)
            implementation->WaitForIdle();
    }
    String GraphicsContextBase::GetImplementationName() const
    {
        if (implementation != nullptr)
            return implementation->GetImplementationName();

        return "Unimplemented";
    }
    GraphicsContextBase& GraphicsContextBase::operator=(GraphicsContextBase&& other) noexcept
    {
        implementation = other.implementation;
        other.implementation = nullptr;

        return *this;
    }
    GraphicsContextBase::GraphicsContextBase()
        : implementation(nullptr)
    {
    }

    SemaphoreBase::SemaphoreBase(SemaphoreBase&& other) noexcept
        : implementation(other.implementation)
    { 
        other.implementation = nullptr;
    }
    SemaphoreBase::SemaphoreBase(GraphicsContextBase& graphicsContext)
        : implementation(nullptr)
    {       
        auto graphicsContext_OpenGL = graphicsContext.TryCast<OpenGL::GraphicsContext_OpenGL>();
        
        if (graphicsContext_OpenGL != nullptr)        
            implementation = DefaultAllocator::Create<OpenGL::Semaphore_OpenGL>(*graphicsContext_OpenGL);
        else
            Debug::Logger::LogError("Blaze graphics API", "Unsupported graphics context implementation.");                
    }
    SemaphoreBase::~SemaphoreBase()
    {
        Destroy();
    }
    void SemaphoreBase::Destroy()
    {
        if (implementation != nullptr)
        {
            DefaultAllocator::Destroy(implementation);
            implementation = nullptr;
        }
    }
    SemaphoreBase& SemaphoreBase::operator=(SemaphoreBase&& other) noexcept
    {
        implementation = other.implementation;
        other.implementation = nullptr;

        return *this;        
    }
    SemaphoreBase::SemaphoreBase()
        :implementation(nullptr)
    {
    }

    RenderContextBase::~RenderContextBase()
    {
    }        
    FramebufferBase::~FramebufferBase()
    {
    }    
    RenderWindowFramebufferBase::~RenderWindowFramebufferBase()
    {
    }

    RenderWindowBase::~RenderWindowBase()
    {
        Destroy();
    }
    RenderWindowBase::RenderWindowBase(RenderWindowBase&& other) noexcept
        : implementation(other.implementation)
    {
        other.implementation = nullptr;
    }
    RenderWindowBase::RenderWindowBase(GraphicsContextBase& graphicsContext, String title, Vec2i pos, Vec2i size)
        : implementation(nullptr)
    {
        auto graphicsContext_OpenGL = graphicsContext.TryCast<OpenGL::GraphicsContext_OpenGL>();

        if (graphicsContext_OpenGL != nullptr)        
            implementation = DefaultAllocator::Create<OpenGL::RenderWindow_OpenGL>(*graphicsContext_OpenGL, title, pos, size);
        else
            Debug::Logger::LogError("Blaze graphics API", "Unsupported graphics context implementation.");        
    }    
    void RenderWindowBase::Destroy()
    {
        if (implementation != nullptr)
        {            
            DefaultAllocator::Destroy(implementation);
            implementation = nullptr;
        }
    }
    WindowBase* RenderWindowBase::GetWindow() const
    {
        if (implementation != nullptr)
            return implementation->GetWindow();
        return nullptr;
    }
    Vec2i RenderWindowBase::GetSize() const
    {
        if (implementation != nullptr)
            return implementation->GetSize();
        return Vec2i();
    }
    RenderWindowFramebufferBase& RenderWindowBase::AcquireNextFramebuffer(SemaphoreBase& semaphore)
    {
        if (implementation != nullptr)
            return implementation->AcquireNextFramebuffer(semaphore);
        return *(RenderWindowFramebufferBase*)nullptr;
    }    
    RenderWindowBase& RenderWindowBase::operator=(RenderWindowBase&& other) noexcept
    {
        implementation = other.implementation;
        other.implementation = nullptr;

        return *this;
    }
    RenderWindowBase::RenderWindowBase()
        : implementation(nullptr)
    {

    }
}