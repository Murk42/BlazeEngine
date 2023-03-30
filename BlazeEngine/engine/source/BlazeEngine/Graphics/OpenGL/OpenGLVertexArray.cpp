#include "BlazeEngine/Graphics/OpenGL/OpenGLVertexArray.h"
#include "GL/glew.h"

#include "BlazeEngine/Graphics/GraphicsCore.h"

namespace Blaze
{    
    namespace OpenGL
    {
        static bool firstVertexArray = true;

        template<typename T>
        inline std::underlying_type_t<T> ToInteger(T value)
        {
            return static_cast<std::underlying_type_t<T>>(value);
        }

        GLenum OpenGLVertexAttributeType(VertexAttributeType type)
        {
            switch (type)
            {
            case Blaze::OpenGL::VertexAttributeType::Int8: return GL_BYTE;                
            case Blaze::OpenGL::VertexAttributeType::Uint8: return GL_UNSIGNED_BYTE;                
            case Blaze::OpenGL::VertexAttributeType::Int16: return GL_SHORT;                
            case Blaze::OpenGL::VertexAttributeType::Uint16: return GL_UNSIGNED_SHORT;                
            case Blaze::OpenGL::VertexAttributeType::Int32: return GL_INT;                
            case Blaze::OpenGL::VertexAttributeType::Uint32: return GL_UNSIGNED_INT;
            case Blaze::OpenGL::VertexAttributeType::Float: return GL_FLOAT;       
            case Blaze::OpenGL::VertexAttributeType::Double: return GL_DOUBLE;
            }
            Logger::AddLog(BLAZE_FATAL_LOG("Blaze Engine",
                "Invalid VertexAttributeType enum value. The integer value was: " + StringParsing::Convert(ToInteger(type)).value));
            return std::numeric_limits<GLenum>::max();
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

        void VertexArray::SetVertexAttributeBuffer(uint index, const GraphicsBuffer* buffer, uint stride, uint offset)
        {   
            Graphics::Core::SelectVertexArray(this);
            if (buffer != nullptr)
                glBindVertexBuffer(index, buffer->GetHandle(), static_cast<GLintptr>(offset), static_cast<GLsizei>(stride));
            else  
                glBindVertexBuffer(index, 0, offset, stride);
        }

        void VertexArray::SetVertexAttributeFormat(uint index, VertexAttributeType type, uint count, bool normalised, uint offset)
        {
            glVertexArrayAttribFormat(id, index, static_cast<GLint>(count), OpenGLVertexAttributeType(type), normalised, offset);
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