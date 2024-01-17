#include "pch.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLContext.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLConversions.h"

namespace Blaze::Graphics::OpenGLWrapper
{
    static bool firstVertexArray = true;

    template<typename T>
    inline std::underlying_type_t<T> ToInteger(T value)
    {
        return static_cast<std::underlying_type_t<T>>(value);
    }
    VertexArray::VertexArray()
        : id(-1), indexBuffer(nullptr)
    {
        if (firstVertexArray)
        {
            id = 0;
            firstVertexArray = false;
        }
        else
        {
            glCreateVertexArrays(1, &id);
            
        }
    }
    VertexArray::VertexArray(VertexArray&& va) noexcept
        : id(-1), indexBuffer(nullptr)
    {
        id = va.id;
        indexBuffer = va.indexBuffer;
        va.id = -1;
        va.indexBuffer = nullptr;
    }
    VertexArray::~VertexArray()
    {
        if (id != -1 && id != 0)
        {
            glDeleteVertexArrays(1, &id);
            
        }
    }

    GraphicsBuffer* VertexArray::GetIndexBuffer() const
    {
        return indexBuffer;
    }

    Result VertexArray::EnableVertexAttribute(uint index)
    {
        glEnableVertexArrayAttrib(id, index);
        CHECK_OPENGL_RESULT();

        return Result();
    }

    Result VertexArray::DisableVertexAttribute(uint index)
    {
        glDisableVertexArrayAttrib(id, index);
        CHECK_OPENGL_RESULT();

        return Result();
    }

    Result VertexArray::SetVertexAttributeBuffer(uint index, const GraphicsBuffer* buffer, uint stride, uint offset)
    {
        CHECK_RESULT(SelectVertexArray(this));

        if (buffer != nullptr)
            glBindVertexBuffer(index, buffer->GetHandle(), static_cast<GLintptr>(offset), static_cast<GLsizei>(stride));
        else
            glBindVertexBuffer(index, 0, offset, stride);

        CHECK_OPENGL_RESULT();

        return Result();
    }

    Result VertexArray::SetVertexAttributeFormat(uint index, VertexAttributeType type, uint count, bool normalised, uint offset)
    {
        Result result;
        auto attributeType = OpenGLVertexAttributeType(type, result);
        CHECK_RESULT(result);

        glVertexArrayAttribFormat(id, index, static_cast<GLint>(count), attributeType, normalised, offset);
        CHECK_OPENGL_RESULT();

        return Result();
    }

    Result VertexArray::SetVertexAttributeDivisor(uint index, uint divisor)
    {
        glVertexArrayBindingDivisor(id, index, divisor);
        return Result();
    }


    Result VertexArray::SetIndexBuffer(GraphicsBuffer& buffer)
    {
        glVertexArrayElementBuffer(id, buffer.GetHandle());
        CHECK_OPENGL_RESULT();

        indexBuffer = &buffer;
        return Result();
    }

    VertexArray& VertexArray::operator=(VertexArray&& va) noexcept
    {
        if (id != -1)
        {
            glDeleteVertexArrays(1, &id);

            if (FlushOpenGLResult())
                return *this;
        }
        id = va.id;
        indexBuffer = va.indexBuffer;
        va.id = -1;
        va.indexBuffer = nullptr;
        return *this;
    }
}