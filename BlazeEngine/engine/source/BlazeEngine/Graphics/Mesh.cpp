#include "BlazeEngine/Graphics/Mesh.h"

namespace Blaze
{
	Mesh::Mesh()
	{
		vl.BindVertexBuffer(&vb);
		vl.BindIndexBuffer(&ib);
	}

	void Mesh::SetVertices(const void* ptr, uint count, const std::vector<Type>& layout)
	{
		vl.SetLayout(layout);
		vb.AllocateData(ptr, count * vl.GetStride(), Dynamic | Draw);
	}
	void Mesh::ChangeVertices(const void* ptr, uint count, uint offset)
	{
		const uint stride = vl.GetStride();
		vb.ChangeData(ptr, count * stride, offset * stride);
	}

	void Mesh::SetIndicies(const void* ptr, uint count, Type indexType)
	{
		ib.AllocateData(ptr, count * SizeOf(indexType), Static | Draw, indexType);
	}
	void Mesh::ChangeIndicies(const void* ptr, uint count, uint offset)
	{
		const auto indexSize = SizeOf(ib.GetIndexType());
		ib.ChangeData(ptr, count * indexSize, offset * indexSize);
	}
}