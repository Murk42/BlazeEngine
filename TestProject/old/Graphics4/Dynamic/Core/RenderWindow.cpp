#include "pch.h"
#include "RenderWindow.h"

namespace Graphics4::Dynamic
{
    RenderWindowBase::RenderWindowBase(RenderWindowBase&& other) noexcept
        : GraphicsDynamicImplementation(std::move(other))
    {
    }
    RenderWindowBase::RenderWindowBase(GraphicsContextBase& graphicsContext, StringView title, Vec2i pos, Vec2u size)
        : GraphicsDynamicImplementation(graphicsContext, std::move(title), std::move(pos), std::move(size))
    {        
    }
    RenderWindowBase::~RenderWindowBase()
    {
    }
    bool RenderWindowBase::GetWindow(WindowBase*& window) const
    {
        if (auto impl = GetImplementation())
            return impl->GetWindow(window);

        return false;
    }
    Vec2u RenderWindowBase::GetSize() const
    {
        if (auto impl = GetImplementation())
            return impl->GetSize();

        return Vec2u();
    }
    bool RenderWindowBase::AcquireNextFramebuffer(RenderWindowFramebufferBase*& framebuffer, SemaphoreBase& semaphore)
    {
        if (auto impl = GetImplementation())
            return impl->AcquireNextFramebuffer(framebuffer, semaphore);

        return false;
    }
    void RenderWindowBase::PresentFramebuffer(RenderWindowFramebufferBase& framebuffer, Array<SemaphoreBase*> semaphores)
    {
        if (auto impl = GetImplementation())
            impl->PresentFramebuffer(framebuffer, std::move(semaphores));
        return;
    }
    RenderWindowBase& RenderWindowBase::operator=(RenderWindowBase&& other) noexcept
    {
        GraphicsDynamicImplementation::operator=(std::move(other));

        return *this;
    }
    RenderWindowBase::RenderWindowBase()
    {

    }
}