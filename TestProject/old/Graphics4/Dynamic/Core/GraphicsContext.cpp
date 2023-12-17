#include "pch.h"
#include "GraphicsContext.h"

namespace Graphics4::Dynamic
{
    static Map<String, void(*)(GraphicsContextBase*)> implementations;

    GraphicsContextBase::GraphicsContextBase(GraphicsContextBase&& other) noexcept
        : DynamicImplementation(std::move(other))
    {
    }
    GraphicsContextBase::GraphicsContextBase(StringView name)
    {
        auto it = implementations.Find(name);

        if (it.IsNull())
            return;
        
        it->value(this);        
    }
    GraphicsContextBase::~GraphicsContextBase()
    {
    }
    void GraphicsContextBase::WaitForIdle() const
    {
        if (auto impl = GetImplementation())
            impl->WaitForIdle();
    }
    String GraphicsContextBase::GetImplementationName() const
    {
        if (auto impl = GetImplementation())
            return impl->GetImplementationName();

        return String();
    }
    GraphicsContextBase& GraphicsContextBase::operator=(GraphicsContextBase&& other) noexcept
    {
        DynamicImplementation::operator=(std::move(other));
        return *this;
    } 
    bool GraphicsContextBase::AddImplementation(StringView name, void(*impl)(GraphicsContextBase*))
    {
        return implementations.Insert(name, impl).inserted;
    }
    GraphicsContextBase::GraphicsContextBase()
    {
    }
}