#pragma once
#define BLAZE_ONLY_CORE
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

#include "Renderable.h"
#include "Camera.h"

class Skybox : public Renderable
{
	inline static VertexArray* va;
	inline static IndexBuffer* ib;
public:
	TextureCubemap* tex;
	Material* mat;

	Skybox()
	{

	}
	Skybox(TextureCubemap* tex, Material* mat)
		: tex(tex), mat(mat)
	{

	}

	static void Setup()
	{
		static float vertex[]
		{
			 1.0,-1.0,-1.0,
			-1.0, 1.0,-1.0,
			-1.0,-1.0, 1.0,
			 1.0, 1.0, 1.0,
			 1.0,-1.0,-1.0,
			-1.0, 1.0,-1.0,
			-1.0,-1.0, 1.0,
			 1.0, 1.0, 1.0
		};
		static unsigned short index[]
		{
			0, 1, 2,
			2, 3, 0,			
			1, 5, 6,
			6, 2, 1,			
			7, 6, 5,
			5, 4, 7,			
			4, 0, 3,
			3, 7, 4,			
			4, 5, 1,
			1, 0, 4,			
			3, 2, 6,
			6, 7, 3
		};

		va = new VertexArray;
		ib = new IndexBuffer;

		va->AllocateData(vertex, 8, Static | Draw, { Type::Vec3f });
		ib->AllocateData(index, 36, Static | Draw, Type::UnsignedShort);
	}

	static void Cleanup()
	{
		delete va;
		delete ib;
	}


	void Render(Camera* cam, Transform* tran = nullptr) override
	{
		mat->GetProperty("u_texture")->SetData(tex);
		mat->GetProperty("u_proj")->SetData(cam->projMat);
		mat->GetProperty("u_view")->SetData(cam->viewMat);
		mat->UpdateProperties();
		mat->GetShaderSelection()->Bind();

		va->Bind();
		ib->Bind();

		Renderer::DrawTriangles();
	}
};