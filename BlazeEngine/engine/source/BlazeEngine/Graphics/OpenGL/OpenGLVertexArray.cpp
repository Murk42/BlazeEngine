#include "BlazeEngine/Graphics/OpenGL/OpenGLVertexArray.h"
#include "GL/glew.h"


namespace Blaze
{    
    namespace OpenGL
    {
        static bool firstVertexArray = true;

        VertexArray::VertexArray()
            : id(-1), indexBuffer(nullptr)
        {
            if (firstVertexArray)
            {
                id = 0;
                firstVertexArray = false;
            }
            else
                glCreateVertexArrays(1, &id);
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
                glDeleteVertexArrays(1, &id);
        }

        GraphicsBuffer* VertexArray::GetIndexBuffer() const
        {
            return indexBuffer;
        }        

        void VertexArray::EnableVertexAttribute(uint index)
        {
            glEnableVertexArrayAttrib(id, index);
        }

        void VertexArray::DisableVertexAttribute(uint index)
        {
            glDisableVertexArrayAttrib(id, index);
        }

        void VertexArray::SetVertexAttributeBuffer(uint index, const GraphicsBuffer* buffer, size_t stride, size_t offset)
        {
            if (buffer != nullptr)
                glVertexArrayVertexBuffer(id, index, buffer->GetHandle(), offset, stride);
            else
                glVertexArrayVertexBuffer(id, index, 0, offset, stride);
        }

        void VertexArray::SetVertexAttributeFormat(uint index, VertexAttributeType type, size_t count, bool normalised, size_t offset)
        {
            glVertexArrayAttribFormat(id, index, count, (size_t)type, normalised, offset);
        }              


        void VertexArray::SetIndexBuffer(GraphicsBuffer& buffer)
        {                  
            glVertexArrayElementBuffer(id, buffer.id);
            indexBuffer = &buffer;
        }        

        VertexArray& VertexArray::operator=(VertexArray&& va) noexcept
        {
            if (id != -1)
                glDeleteVertexArrays(1, &id);
            id = va.id;
            indexBuffer = va.indexBuffer;
            va.id = -1;
            va.indexBuffer = nullptr;
            return *this;
        }
    }
}