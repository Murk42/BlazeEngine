#include "BlazeEngine/Graphics/OpenGL/VertexArray.h"
#include "GL/glew.h"

#include "source/BlazeEngine/Internal/Engine.h"

namespace Blaze
{    
    namespace OpenGL
    {
        inline VertexArray::VertexArray()
            : id(0), indexBuffer(nullptr)
        {
            if (engine == nullptr)
                id = 0;
            else
                glCreateVertexArrays(1, &id);
        }        
        inline VertexArray::VertexArray(VertexArray&& va) noexcept
            : id(0), indexBuffer(nullptr)
        {
            id = va.id;
            indexBuffer = va.indexBuffer;
            va.id = 0;
            va.indexBuffer = nullptr;
        }
        inline VertexArray::~VertexArray()
        {
            glDeleteVertexArrays(1, &id);
        }

        inline BaseBuffer* VertexArray::GetIndexBuffer() const
        {
            return indexBuffer;
        }

        inline void VertexArray::EnableVertexAttribute(uint index)
        {
            glEnableVertexArrayAttrib(id, index);
        }

        inline void VertexArray::DisableVertexAttribute(uint index)
        {
            glDisableVertexArrayAttrib(id, index);
        }

        inline void VertexArray::VertexAttributeVertexBuffer(uint index, const BaseBuffer& buffer, size_t stride, size_t offset)
        {
            glVertexArrayVertexBuffer(id, index, buffer.id, offset, stride);
        }

        inline void VertexArray::VertexAttributeFormat(uint index, AttributeType type, bool normalised, size_t offset)
        {
            int _count = 0;
            uint _type = 0;
            switch (type)
            {
            case Blaze::OpenGL::AttributeType::Float: _count = 1; _type = GL_FLOAT; break;
            case Blaze::OpenGL::AttributeType::Vec2f: _count = 2; _type = GL_FLOAT; break;
            case Blaze::OpenGL::AttributeType::Vec3f: _count = 3; _type = GL_FLOAT; break;
            case Blaze::OpenGL::AttributeType::Vec4f: _count = 4; _type = GL_FLOAT; break;
            }
            
            glVertexArrayAttribFormat(id, index, _count, _type, normalised, offset);
        }

        inline void VertexArray::SetIndexBuffer(BaseBuffer& buffer)
        {            
            glVertexArrayElementBuffer(id, buffer.id);
            indexBuffer = &buffer;
        }

        inline void VertexArray::Bind(const VertexArray& va)
        {
            glBindVertexArray(va.id);
            engine->Renderer.boundVertexArray = (VertexArray*)&va;
        }
        inline void VertexArray::Unbind()
        {
            glBindVertexArray(0);
            engine->Renderer.boundVertexArray = &engine->Renderer.defaultVertexArray;
        }
        inline VertexArray* VertexArray::GetBound()
        {
            return engine->Renderer.boundVertexArray;
        }

        inline VertexArray& VertexArray::operator=(VertexArray&& va) noexcept
        {
            id = va.id;
            indexBuffer = va.indexBuffer;
            va.id = 0;
            va.indexBuffer = nullptr;
            return *this;
        }
    }
}