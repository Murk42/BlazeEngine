#include "Star.h"
#include "App.h"

struct Vertex
{
	Vec3f pos;
};

StarMesh::StarMesh()
	: indexCount(0)
{
	va.SetVertexAttributeData(0, VertexAttribute(&vb, sizeof(Vertex), 0, VertexAttributeType::Float, 3, false, 0));
	va.SetIndexBuffer(ib);
}

void StarMesh::Render()
{
	VertexArray::Bind(va);
	Renderer::RenderTriangles(IndexType::UnsignedInt, indexCount);
}


void Star::Initialize(Vec3f pos, size_t size)
{
	this->size = size;
	transform.pos = pos;

	MeshData meshData;
	meshData.Load("assets/world/star/mesh.obj", MeshFileType::OBJ);

	mesh.vb.AllocateStaticStorage(BufferView(meshData.vertexBuffer, meshData.vertexBufferSize), GraphicsBufferStaticStorageHint::DynamicStorage);
	mesh.ib.AllocateStaticStorage(BufferView(meshData.indexBuffer, meshData.indexBufferSize));	
	mesh.indexCount = meshData.indexBufferSize / sizeof(unsigned);
}

void Star::UpdateTransform(Vec3f cameraDelta)
{
	transform.pos -= cameraDelta;
	transform.Update();
}

void Star::Render(const Mat4f& VP3D)
{
	app->starMaterial.size = this->size;
	app->starMaterial.MVP = VP3D * transform.mat;
	app->starMaterial.UpdateProperties();
	app->starProgram.Use();

	mesh.Render();
}