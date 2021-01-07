#pragma once
#define BLAZE_ONLY_CORE
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

#include "Renderable.h"
#include "Transform.h"
#include "Camera.h"

class Model : public Renderable
{
public:
	vector<Material*> materials;
	Mesh* mesh;

	Model()
	{

	}
	Model(const vector<Material*>& materials, Mesh* mesh)
		: materials(materials), mesh(mesh)
	{
	}

	void Render(Camera* cam, Transform* tran) override 
	{
		for (auto& mat : materials)
		{
			Material::Property* MVP_prop = mat->GetProperty("u_MVP");
			if (MVP_prop != nullptr)
				MVP_prop->SetData(cam->projMat * cam->viewMat * tran->mat);			
		}

		mesh->GetVertexArray()->Bind();
		mesh->GetIndexBuffer()->Bind();

		unsigned* indexCounts = mesh->GetIndexMaterialCounts();
		int offset = 0;
		for (int i = 0; i < mesh->GetMaterialCount(); i++)
		{
			materials[i]->UpdateProperties();
			materials[i]->GetShaderSelection()->Bind();

			Renderer::DrawTriangles(indexCounts[i], offset);

			offset += indexCounts[i];
		}
	}
};