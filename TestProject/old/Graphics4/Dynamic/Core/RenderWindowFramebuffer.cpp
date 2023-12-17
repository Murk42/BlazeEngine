#include "pch.h"
#include "RenderWindowFramebuffer.h"

namespace Graphics4::Dynamic
{
    RenderWindowFramebufferBase::RenderWindowFramebufferBase(RenderWindowFramebufferBase&& other) noexcept
        : DynamicImplementation(std::move(other))
    {
    }
    RenderWindowFramebufferBase::~RenderWindowFramebufferBase()
    {
    }
    bool RenderWindowFramebufferBase::GetFramebuffer(FramebufferBase*& framebuffer)
    {
        if (auto impl = GetImplementation())
            return impl->GetFramebuffer(framebuffer);
        return false;
    }
    bool RenderWindowFramebufferBase::GetRenderWindow(RenderWindowBase*& renderWindow) const
    {
        if (auto impl = GetImplementation())
            return impl->GetRenderWindow(renderWindow);
        return false;
    }
    RenderWindowFramebufferBase& RenderWindowFramebufferBase::operator=(RenderWindowFramebufferBase&& other) noexcept
    {
        DynamicImplementation::operator=(std::move(other));

        return *this;
    }
    RenderWindowFramebufferBase::RenderWindowFramebufferBase()
    {
    }
}