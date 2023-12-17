#include "pch.h"
#include "Framebuffer.h"

namespace Graphics4::Dynamic
{
    FramebufferBase::FramebufferBase(FramebufferBase&& other) noexcept
        : DynamicImplementation(std::move(other))
    {
    }
    FramebufferBase::~FramebufferBase()
    {
    }
    Vec2u FramebufferBase::GetSize() const
    {
        if (auto impl = GetImplementation())
            return impl->GetSize();
        return Vec2u();
    }
    FramebufferBase& FramebufferBase::operator=(FramebufferBase&& other) noexcept
    {
        DynamicImplementation::operator=(std::move(other));
        return *this;
    }
    FramebufferBase::FramebufferBase()
    {
    }
}