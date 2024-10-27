#include "pch.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLVertexArray.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLGraphicsBuffer.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLConversions.h"

namespace Blaze::Graphics::OpenGLWrapper
{
    template<typename T>
    inline std::underlying_type_t<T> ToInteger(T value)
    {
        return static_cast<std::underlying_type_t<T>>(value);
    }
    VertexArray::VertexArray()
        : id(0)
    {     
        glCreateVertexArrays(1, &id);
    }
    VertexArray::VertexArray(uint id)
        : id(id)
    {
    }
    VertexArray::VertexArray(VertexArray&& va) noexcept
        : id(0)
    {
        id = va.id;        
        va.id = 0;        
    }
    VertexArray::~VertexArray()
    {
        if (id != 0)        
            glDeleteVertexArrays(1, &id);
    }    

    void VertexArray::EnableVertexAttribute(uint index)
    {
        glEnableVertexArrayAttrib(id, index);        
    }

    void VertexArray::DisableVertexAttribute(uint index)
    {
        glDisableVertexArrayAttrib(id, index);        
    }

    void VertexArray::SetVertexAttributeBuffer(uint index, const GraphicsBuffer* buffer, uint stride, uint offset)
    {
        if (buffer != nullptr)
            glVertexArrayVertexBuffer(id, index, buffer->GetHandle(), static_cast<GLintptr>(offset), static_cast<GLsizei>(stride));
        else
            glVertexArrayVertexBuffer(id, index, 0, offset, stride);
    }

    void VertexArray::SetVertexAttributeFormat(uint index, VertexAttributeType type, uint count, bool normalised, uint offset)
    {
        Result result;
        auto attributeType = OpenGLVertexAttributeType(type, result);
        if (result) return;

        switch (type)
        {
        case Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Int8:
            glVertexArrayAttribIFormat(id, index, static_cast<GLint>(count), attributeType, offset);
            break;
        case Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Uint8:
            glVertexArrayAttribIFormat(id, index, static_cast<GLint>(count), attributeType, offset);
            break;
        case Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Int16:
            glVertexArrayAttribIFormat(id, index, static_cast<GLint>(count), attributeType, offset);
            break;
        case Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Uint16:
            glVertexArrayAttribIFormat(id, index, static_cast<GLint>(count), attributeType, offset);
            break;
        case Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Int32:
            glVertexArrayAttribIFormat(id, index, static_cast<GLint>(count), attributeType, offset);
            break;
        case Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Uint32:
            glVertexArrayAttribIFormat(id, index, static_cast<GLint>(count), attributeType, offset);
            break;
        case Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float:
            glVertexArrayAttribFormat(id, index, static_cast<GLint>(count), attributeType, normalised, offset);
            break;
        case Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Double:
            glVertexArrayAttribLFormat(id, index, static_cast<GLint>(count), attributeType, offset);
            break;        
        }        
    }

    void VertexArray::SetVertexAttributeDivisor(uint index, uint divisor)
    {
        glVertexArrayBindingDivisor(id, index, divisor);        
    }

    void VertexArray::Swap(VertexArray& other)
    {
        uint temp = id;
        id = other.id;
        other.id = temp;        
    }

    uint VertexArray::ReleaseHandleOwnership()
    {
        uint id = this->id;
        this->id = 0;
        return id;
    }


    void VertexArray::SetIndexBuffer(GraphicsBuffer& buffer)
    {
        glVertexArrayElementBuffer(id, buffer.GetHandle());        
    }

    VertexArray& VertexArray::operator=(VertexArray&& va) noexcept
    {
        if (id != 0)
            glDeleteVertexArrays(1, &id);

        id = va.id;
        va.id = 0;
        return *this;
    }
}