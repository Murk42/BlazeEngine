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
    void VertexArray::SetVertexAttribute(const VertexAttributeDescriptor& descriptor)
    {        
        for (uintMem i = 0; i < descriptor.arraySize; ++i)
        {
            uint index = descriptor.attributeIndex + static_cast<uint>(i);
            uintMem offset = descriptor.offset + i * descriptor.elementSize;

            EnableVertexAttribute(index);
            SetVertexAttributeBinding(index, descriptor.bindingIndex);
            SetVertexAttributeDivisor(index, descriptor.divisor);

            descriptor.type.Visit(Visitor{
                [&](const IntegerVertexAttributeType& type)    { SetIntegerVertexAttributeFormat           (index, type, descriptor.count, offset); },
                [&](const DoubleVertexAttributeType& type)     { SetDoubleVertexAttributeFormat            (index, type, descriptor.count, offset); },
                [&](const FloatVertexAttributeType& type)      { SetFloatVertexAttributeFormat             (index, type, descriptor.count, offset); },
                [&](const NormalizedVertexAttributeType& type) { SetFloatVertexAttributeAsNormalizedFormat (index, type, descriptor.count, offset); },
                [&](const PackedVertexAttributeType& type)     { SetFloatVertexAttributeAsPackedFormat     (index, type, descriptor.count, offset); },
                [&](const BGRAVertexAttributeType& type)       { SetFloatVertexAttributeAsBGRAFormat       (index, type, descriptor.count, offset); },
                });
        }
    }
    void VertexArray::SetIntegerVertexAttributeFormat(uint index, IntegerVertexAttributeType type, uintMem count, uintMem offset)
    {
        GLenum _type;
        if (!OpenGLIntegerVertexAttributeType(type, _type))
        {
            BLAZE_LOG_ERROR("Invalid IntegerVertexAttributeType enum value");
            return;
        }
        glVertexArrayAttribIFormat(id, index, count, _type, offset);
    }
    void VertexArray::SetDoubleVertexAttributeFormat(uint index, DoubleVertexAttributeType type, uintMem count, uintMem offset)
    {
        GLenum _type;
        if (!OpenGLDoubleVertexAttributeType(type, _type))
        {
            BLAZE_LOG_ERROR("Invalid DoubleVertexAttributeType enum value");
            return;
        }
        glVertexArrayAttribLFormat(id, index, count, _type, offset);
    }
    void VertexArray::SetFloatVertexAttributeFormat(uint index, FloatVertexAttributeType type, uintMem count, uintMem offset)
    {
        GLenum _type;
        if (!OpenGLFloatVertexAttributeType(type, _type))
        {
            BLAZE_LOG_ERROR("Invalid FloatVertexAttributeType enum value");
            return;
        }
        glVertexArrayAttribFormat(id, index, count, _type, false, offset);
    }
    void VertexArray::SetFloatVertexAttributeAsNormalizedFormat(uint index, NormalizedVertexAttributeType type, uintMem count, uintMem offset)
    {
        GLenum _type;
        if (!OpenGLNormalizedVertexAttributeType(type, _type))
        {
            BLAZE_LOG_ERROR("Invalid IntegerVertexAttributeType enum value");
            return;
        }
        glVertexArrayAttribFormat(id, index, count, _type, true, offset);
    }
    void VertexArray::SetFloatVertexAttributeAsPackedFormat(uint index, PackedVertexAttributeType type, uintMem count, uintMem offset)
    {
        GLenum _type;
        if (!OpenGLPackedVertexAttributeType(type, _type))
        {
            BLAZE_LOG_ERROR("Invalid PackedVertexAttributeType enum value");
            return;
        }
        glVertexArrayAttribFormat(id, index, 4, _type, true, offset);
    }
    void VertexArray::SetFloatVertexAttributeAsBGRAFormat(uint index, BGRAVertexAttributeType type, uintMem count, uintMem offset)
    {
        GLenum _type;
        if (!OpenGLBGRAVertexAttributeType(type, _type))
        {
            BLAZE_LOG_ERROR("Invalid BGRAVertexAttributeType enum value");
            return;
        }
        glVertexArrayAttribFormat(id, index, GL_BGRA, _type, true, offset);
    }
    void VertexArray::SetBindingBuffer(uint bindingIndex, const GraphicsBuffer* buffer, uintMem stride, uintMem offset)
    {
        if (buffer != nullptr)
            glVertexArrayVertexBuffer(id, bindingIndex, buffer->GetHandle(), static_cast<GLintptr>(offset), static_cast<GLsizei>(stride));
        else
            glVertexArrayVertexBuffer(id, bindingIndex, 0, offset, stride);
    }
    void VertexArray::SetVertexAttributeBinding(uint attributeIndex, uint bindingIndex)
    {
        glVertexArrayAttribBinding(id, attributeIndex, bindingIndex);
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