#include "pch.h"
#include "Semaphore.h"

namespace Graphics4::Dynamic
{
    SemaphoreBase::SemaphoreBase(SemaphoreBase&& other) noexcept
        : GraphicsDynamicImplementation(std::move(other))
    {
    }
    SemaphoreBase::SemaphoreBase(GraphicsContextBase& graphicsContext)
        : GraphicsDynamicImplementation(graphicsContext)
    {        
    }
    SemaphoreBase::~SemaphoreBase()
    {
    }
    SemaphoreBase& SemaphoreBase::operator=(SemaphoreBase&& other) noexcept
    {
        GraphicsDynamicImplementation::operator=(std::move(other));
        return *this;
    }
    SemaphoreBase::SemaphoreBase()
    {
    }
}