#include "BlazeEngine/Graphics/VertexLayout.h"
#include "BlazeEngine/Graphics/Buffer.h"

#include "source/BlazeEngine/Internal/Engine.h"

#include "GL/glew.h"

namespace Blaze
{		
	VertexLayout::VertexLayout()
		: vertexBuffer(nullptr), indexBuffer(nullptr)
	{
		if (engine == nullptr)
			id = 0;
		else	
			glGenVertexArrays(1, &id);
	}
	VertexLayout::VertexLayout(const VertexLayout& vl)
	{
		glGenVertexArrays(1, &id);
		SetLayout(vl.layout);
		BindVertexBuffer(vl.vertexBuffer);
		BindIndexBuffer(vl.indexBuffer);
	}
	VertexLayout::VertexLayout(VertexLayout&& vl) noexcept
		: id(std::exchange(vl.id, 0)), vertexBuffer(vl.vertexBuffer), indexBuffer(vl.indexBuffer)
	{

	}
	VertexLayout::VertexLayout(const std::vector<Type>& layout, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer)
	{
		glGenVertexArrays(1, &id);
		SetLayout(layout);
		BindVertexBuffer(vertexBuffer);
		BindIndexBuffer(indexBuffer);
	}
	VertexLayout::~VertexLayout()
	{		
		glDeleteVertexArrays(1, &id);
	}
	void VertexLayout::BindVertexBuffer(VertexBuffer* vb)
	{
		glBindVertexArray(id);
		glBindBuffer(GL_ARRAY_BUFFER, ((Buffer*)vb)->id);
		vertexBuffer = vb;
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void VertexLayout::BindIndexBuffer(IndexBuffer* ib)
	{
		glBindVertexArray(id);		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ((Buffer*)ib)->id);
		indexBuffer = ib;
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	void VertexLayout::SetLayout(const std::vector<Type>& layout)
	{
		glBindVertexArray(id);
		glBindBuffer(GL_ARRAY_BUFFER, ((Buffer*)vertexBuffer)->id);

		size_t stride = 0;
		for (auto& t : layout) stride += SizeOf(t);
		this->layout = layout;		

		size_t offset = 0;
		for (int i = 0; i < layout.size(); i++)
		{
			Type type = (Type)0;
			int count = 0;			
			switch (layout[i])
			{
			case Type::Vec2b:         type = Type::Bool;          count = 2; break;
			case Type::Vec2u:         type = Type::UnsignedInt;   count = 2; break;
			case Type::Vec2i:         type = Type::Int;           count = 2; break;
			case Type::Vec2f:         type = Type::Float;         count = 2; break;
			case Type::Vec2d:         type = Type::Double;        count = 2; break;
																			 
			case Type::Vec3b:         type = Type::Bool;          count = 3; break;
			case Type::Vec3u:         type = Type::UnsignedInt;   count = 3; break;
			case Type::Vec3i:         type = Type::Int;           count = 3; break;
			case Type::Vec3f:         type = Type::Float;         count = 3; break;
			case Type::Vec3d:         type = Type::Double;        count = 3; break;
																			 
			case Type::Vec4b:         type = Type::Bool;          count = 4; break;
			case Type::Vec4u:         type = Type::UnsignedInt;   count = 4; break;
			case Type::Vec4i:         type = Type::Int;           count = 4; break;
			case Type::Vec4f:         type = Type::Float;         count = 4; break;
			case Type::Vec4d:         type = Type::Double;        count = 4; break;
																			 
			case Type::Bool:          type = Type::Bool;          count = 1; break;
			case Type::Byte:          type = Type::Byte;          count = 1; break;
			case Type::UnsignedByte:  type = Type::UnsignedByte;  count = 1; break;
			case Type::Short:         type = Type::Short;         count = 1; break;
			case Type::UnsignedShort: type = Type::UnsignedShort; count = 1; break;
			case Type::Int:           type = Type::Int;           count = 1; break;
			case Type::UnsignedInt:   type = Type::UnsignedInt;   count = 1; break;
			case Type::Float:         type = Type::Float;         count = 1; break;
			case Type::Double:        type = Type::Double;        count = 1; break;
			}

			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, count, (int)type, GL_FALSE, stride, (const void*)offset);

			offset += SizeOf(layout[i]);
		}

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void VertexLayout::Bind() const
	{
		glBindVertexArray(id);
		engine->Renderer.boundVertexLayout = (VertexLayout*)this;
	}
	void VertexLayout::Unbind()
	{
		glBindVertexArray(0);
		engine->Renderer.boundVertexLayout = &engine->Renderer.defaultVAO;
	}
	VertexLayout* VertexLayout::GetBound()
	{
		return engine->Renderer.boundVertexLayout;
	}
	void VertexLayout::operator=(const VertexLayout& vl)
	{
		SetLayout(vl.layout);
		BindVertexBuffer(vl.vertexBuffer);
		BindIndexBuffer(vl.indexBuffer);
	}
	void VertexLayout::operator=(VertexLayout&& vl) noexcept
	{
		glDeleteVertexArrays(1, &id);
		id = std::exchange(vl.id, 0);
		vertexBuffer = vl.vertexBuffer;
		indexBuffer = vl.indexBuffer;
	}
}