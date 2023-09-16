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
    GraphicsContext CreateGraphicsContext(ImplementationInfo* info)
    {
        switch (info->api)
        {
        case ImplementationAPI::Vulkan:
            return GraphicsContext(new Vulkan::GraphicsContext_Vulkan(std::move(Vulkan::CreateGraphicsContext(info))), info);            
        case ImplementationAPI::OpenGL:
            return GraphicsContext(new OpenGL::GraphicsContext_OpenGL(std::move(OpenGL::CreateGraphicsContext(info))), info);            
        case ImplementationAPI::Other:            
            break;
        default:            
            Debug::Logger::LogFatal("Blaze graphics API", "Invalid implementation API value.");
            break;
        }            
    }
    Semaphore CreateSemaphore(GraphicsContext& graphicsContext)
    {
        auto info = graphicsContext.GetImplementationInfo();
        switch (info->api)
        {
        case ImplementationAPI::Vulkan:
            return Semaphore(new Vulkan::Semaphore(std::move(Vulkan::CreateSemaphore((Vulkan::GraphicsContext&)*graphicsContext.GetPtr()))), info);
            break;
        case ImplementationAPI::OpenGL:
            return Semaphore(new OpenGL::Semaphore(std::move(OpenGL::CreateSemaphore((OpenGL::GraphicsContext&)*graphicsContext.GetPtr()))), info);
        case ImplementationAPI::Other:
            break;
        default:
            Debug::Logger::LogFatal("Blaze graphics API", "Invalid implementation API value.");
            break;
        }
    }
    RenderContext Dynamic::CreateGraphicsRenderContext(GraphicsContext& graphicsContext)
    {
        auto info = graphicsContext.GetImplementationInfo();
        switch (info->api)
        {
        case ImplementationAPI::Vulkan:
            return RenderContext(new Vulkan::RenderContext_Vulkan(std::move(Vulkan::CreateGraphicsRenderContext((Vulkan::GraphicsContext_Vulkan&)*graphicsContext.GetPtr()))), info);
            break;
        case ImplementationAPI::OpenGL:
            return RenderContext(new OpenGL::RenderContext_OpenGL(std::move(OpenGL::CreateGraphicsRenderContext((OpenGL::GraphicsContext_OpenGL&)*graphicsContext.GetPtr()))), info);
        case ImplementationAPI::Other:
            break;
        default:
            Debug::Logger::LogFatal("Blaze graphics API", "Invalid implementation API value.");
            break;
        }
    }
    RenderContext Dynamic::CreatePresentRenderContext(GraphicsContext& graphicsContext)
    {
        auto info = graphicsContext.GetImplementationInfo();
        switch (info->api)
        {
        case ImplementationAPI::Vulkan:
            return RenderContext(new Vulkan::RenderContext_Vulkan(std::move(Vulkan::CreatePresentRenderContext((Vulkan::GraphicsContext_Vulkan&)*graphicsContext.GetPtr()))), info);
            break;
        case ImplementationAPI::OpenGL:
            return RenderContext(new OpenGL::RenderContext_OpenGL(std::move(OpenGL::CreatePresentRenderContext((OpenGL::GraphicsContext_OpenGL&)*graphicsContext.GetPtr()))), info);
        case ImplementationAPI::Other:
            break;
        default:
            Debug::Logger::LogFatal("Blaze graphics API", "Invalid implementation API value.");
            break;
        }
    }    
    RenderWindow CreateRenderWindow(GraphicsContext& graphicsContext, String title, Vec2i size)
    {
        auto info = graphicsContext.GetImplementationInfo();
        switch (info->api)
        {
        case ImplementationAPI::Vulkan:
            return RenderWindow(new Vulkan::RenderWindow_Vulkan(std::move(Vulkan::CreateRenderWindow((Vulkan::GraphicsContext_Vulkan&)*graphicsContext.GetPtr(), title, size))), info);
            break;
        case ImplementationAPI::OpenGL:
            return RenderWindow(new OpenGL::RenderWindow_OpenGL(std::move(OpenGL::CreateRenderWindow((OpenGL::GraphicsContext_OpenGL&)*graphicsContext.GetPtr(), title, size))), info);
        case ImplementationAPI::Other:
            break;
        default:
            Debug::Logger::LogFatal("Blaze graphics API", "Invalid implementation API value.");
            break;
        }
    }
    RenderWindowFramebufferBase& AquireNextFramebuffer(RenderWindow& renderWindow, Semaphore& signalSemaphore)
    {
        auto info = renderWindow.GetImplementationInfo();
        switch (info->api)
        {
        case ImplementationAPI::Vulkan:
            return Vulkan::AquireNextFramebuffer((Vulkan::RenderWindow&)*renderWindow.GetPtr(), (Vulkan::Semaphore&)*signalSemaphore.GetPtr());
            break;
        case ImplementationAPI::OpenGL:
            return OpenGL::AquireNextFramebuffer((OpenGL::RenderWindow&)*renderWindow.GetPtr(), (OpenGL::Semaphore&)*signalSemaphore.GetPtr());
        case ImplementationAPI::Other:
            return OpenGL::AquireNextFramebuffer((OpenGL::RenderWindow&)*renderWindow.GetPtr(), (OpenGL::Semaphore&)*signalSemaphore.GetPtr());
            break;
        default:
            return OpenGL::AquireNextFramebuffer((OpenGL::RenderWindow&)*renderWindow.GetPtr(), (OpenGL::Semaphore&)*signalSemaphore.GetPtr());
            break;
        }
    }
    void PresentRenderWindow(RenderContext& renderContext, RenderWindow& renderWindow, RenderWindowFramebufferBase& framebuffer, std::initializer_list<Semaphore*> waitSemaphores)
    {
        auto info = renderWindow.GetImplementationInfo();
        switch (info->api)
        {
        case ImplementationAPI::Vulkan: {
            Array<Vulkan::Semaphore*> _waitSemaphores { [&](Vulkan::Semaphore** it, uint i) {
                *it = (Vulkan::Semaphore*)waitSemaphores.begin()[i]->GetPtr();
            }, (uint)waitSemaphores.size() };

            return Vulkan::PresentRenderWindow((Vulkan::RenderContext&)*renderContext.GetPtr(), (Vulkan::RenderWindow&)*renderWindow.GetPtr(), (Vulkan::RenderWindowFramebuffer&)framebuffer, std::initializer_list<Vulkan::Semaphore*>(_waitSemaphores.Ptr(), _waitSemaphores.Ptr() + _waitSemaphores.Count()));
            break;
        }
        case ImplementationAPI::OpenGL: {
            Array<OpenGL::Semaphore*> _waitSemaphores { [&](OpenGL::Semaphore** it, uint i) {
                *it = (OpenGL::Semaphore*)waitSemaphores.begin()[i]->GetPtr();
            }, (uint)waitSemaphores.size() };

            return OpenGL::PresentRenderWindow((OpenGL::RenderContext&)*renderContext.GetPtr(), (OpenGL::RenderWindow&)*renderWindow.GetPtr(), (OpenGL::RenderWindowFramebuffer&)framebuffer, std::initializer_list<OpenGL::Semaphore*>(_waitSemaphores.Ptr(), _waitSemaphores.Ptr() + _waitSemaphores.Count()));
        }
        case ImplementationAPI::Other:
            break;
        default:
            Debug::Logger::LogFatal("Blaze graphics API", "Invalid implementation API value.");
            break;
        }
    }    

    GraphicsContextBase::~GraphicsContextBase()
    {
    }    
    SemaphoreBase::~SemaphoreBase()
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
    }
}
