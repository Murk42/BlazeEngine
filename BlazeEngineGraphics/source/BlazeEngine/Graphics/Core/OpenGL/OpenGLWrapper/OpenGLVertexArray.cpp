#include "pch.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLVertexArray.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLGraphicsBuffer.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLConversions.h"

#include <GL/glew.h>

namespace Blaze::Graphics::OpenGL
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
    void VertexArray::SetIndexBuffer(const GraphicsBuffer& buffer)
    {
        glVertexArrayElementBuffer(id, buffer.GetHandle());
    }
    void VertexArray::EnableVertexAttribute(uint index)
    {
        glEnableVertexArrayAttrib(id, index);
    }
    void VertexArray::DisableVertexAttribute(uint index)
    {
        glDisableVertexArrayAttrib(id, index);
    }
    void VertexArray::SetVertexAttributeBuffer(uint index, const GraphicsBuffer* buffer, uintMem stride, uintMem offset)
    {
        if (buffer != nullptr)
            glVertexArrayVertexBuffer(id, index, buffer->GetHandle(), static_cast<GLintptr>(offset), static_cast<GLsizei>(stride));
        else
            glVertexArrayVertexBuffer(id, index, 0, offset, stride);
    }
    void VertexArray::SetIntegerVertexAttributeFormat(uint index, IntegerVertexAttributeType type, uintMem count, uintMem offset)
    {
        GLenum _type = ConvertToOpenGLEnum(type);
        glVertexArrayAttribIFormat(id, index, count, _type, offset);
    }
    void VertexArray::SetDoubleVertexAttributeFormat(uint index, uintMem count, uintMem offset)
    {
        glVertexArrayAttribLFormat(id, index, count, GL_DOUBLE, offset);
    }
    void VertexArray::SetFloatVertexAttributeFormat(uint index, FloatVertexAttributeType type, uintMem count, uintMem offset)
    {
        GLenum _type = ConvertToOpenGLEnum(type);
        glVertexArrayAttribFormat(id, index, count, _type, false, offset);
    }
    void VertexArray::SetFloatVertexAttributeAsNormalizedFormat(uint index, IntegerVertexAttributeType type, uintMem count, uintMem offset)
    {
        GLenum _type = ConvertToOpenGLEnum(type);
        glVertexArrayAttribFormat(id, index, count, _type, true, offset);
    }
    void VertexArray::SetFloatVertexAttributeAsPackedFormat(uint index, PackedVertexAttributeType type, uintMem count, uintMem offset)
    {
        GLenum _type = ConvertToOpenGLEnum(type);
        glVertexArrayAttribFormat(id, index, 4, _type, true, offset);
    }
    void VertexArray::SetFloatVertexAttributeAsBGRAFormat(uint index, BGRAVertexAttributeType type, uintMem count, uintMem offset)
    {
        GLenum _type = ConvertToOpenGLEnum(type);
        glVertexArrayAttribFormat(id, index, GL_BGRA, _type, true, offset);
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
    void VertexArray::Release()
    {
        if (id != 0)
        {
            glDeleteVertexArrays(1, &id);
            id = 0;
        }
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