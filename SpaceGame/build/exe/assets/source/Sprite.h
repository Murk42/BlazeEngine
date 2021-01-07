#pragma once
#define BLAZE_ONLY_CORE
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

#include "Object.h"
#include "Renderable.h"
#include "Camera.h"

class SpriteBatch;

class Sprite : public Object
{	
public:		
	vec2i texPos;
	vec2i texSize;	

	Sprite()
	{

	}
	Sprite(SpriteBatch* batch, const Transform& tran, vec2i texPos, vec2i texSize)
		: Object((Renderable*)batch, tran), texPos(texPos), texSize(texSize)
	{		
	}		
};

class SpriteBatch : public Renderable
{
	float* vertex;	
public:
	VertexArray* va;
	IndexBuffer* ib;
	Material* mat;
	Texture2D* tex;
	vector<Sprite*> sprites;

	SpriteBatch()
	{

	}
	SpriteBatch(Material* mat, Texture2D* tex)
		: mat(mat), tex(tex), va(nullptr), ib(nullptr)
	{
		
	}
	~SpriteBatch()
	{
		if (va != nullptr) delete va;
		if (ib != nullptr) delete ib;
		if (vertex != nullptr) delete[] vertex;
	}

	void AddSprite(Sprite* sprite)
	{
		sprites.push_back(sprite);
	}

	void Update()
	{		
		float* ptr = this->vertex;
		for (int i = 0; i < sprites.size(); i++, ptr += 20)
		{
			*(vec3f*)(ptr + 0) = sprites[i]->tran.pos;
			*(vec2f*)(ptr + 3) = sprites[i]->texPos;

			*(vec3f*)(ptr + 5) = sprites[i]->tran.pos + vec3f(sprites[i]->tran.scale.x, 0, 0);
			*(vec2f*)(ptr + 8) = sprites[i]->texPos + vec2f((float)sprites[i]->texSize.x / tex->GetSize().x, 0);

			*(vec3f*)(ptr + 10) = sprites[i]->tran.pos + vec3f(0, sprites[i]->tran.scale.y, 0);
			*(vec2f*)(ptr + 13) = sprites[i]->texPos + vec2f(0, (float)sprites[i]->texSize.y / tex->GetSize().y);

			*(vec3f*)(ptr + 15) = sprites[i]->tran.pos + vec3f(sprites[i]->tran.scale.x, sprites[i]->tran.scale.y, 0);
			*(vec2f*)(ptr + 18) = sprites[i]->texPos + (vec2f)sprites[i]->texSize / tex->GetSize();
		}

		va->ChangeData(vertex, 4 * sprites.size(), 0);		
	}
	void UpdateMesh()
	{
		if (va != nullptr) delete va;
		if (ib != nullptr) delete ib;
		if (vertex != nullptr) delete[] vertex;		
		
		vertex = new float[(3 + 2) * 4 * sprites.size()];
		unsigned* index = new unsigned[6 * sprites.size()];	

		float* ptrv = this->vertex;
		unsigned* ptri = index;
		for (int i = 0;
			i < sprites.size(); i++, ptrv += 20, ptri += 6)
		{
			*(vec3f*)(ptrv + 0) = sprites[i]->tran.pos;
			*(vec2f*)(ptrv + 3) = sprites[i]->texPos;

			*(vec3f*)(ptrv + 5) = sprites[i]->tran.pos + vec3f(sprites[i]->tran.scale.x, 0, 0);
			*(vec2f*)(ptrv + 8) = sprites[i]->texPos + vec2f((float)sprites[i]->texSize.x / tex->GetSize().x, 0);

			*(vec3f*)(ptrv + 10) = sprites[i]->tran.pos + vec3f(0, sprites[i]->tran.scale.y, 0);
			*(vec2f*)(ptrv + 13) = sprites[i]->texPos + vec2f(0, (float)sprites[i]->texSize.y / tex->GetSize().y);

			*(vec3f*)(ptrv + 15) = sprites[i]->tran.pos + vec3f(sprites[i]->tran.scale.x, sprites[i]->tran.scale.y, 0);
			*(vec2f*)(ptrv + 18) = sprites[i]->texPos + (vec2f)sprites[i]->texSize / tex->GetSize();


			*(ptri + 0) = i * 6 + 0;
			*(ptri + 1) = i * 6 + 2;
			*(ptri + 2) = i * 6 + 1;
			*(ptri + 3) = i * 6 + 1;
			*(ptri + 4) = i * 6 + 2;
			*(ptri + 5) = i * 6 + 3;
		}
		
		va = new VertexArray(vertex, 4 * sprites.size(), Dynamic | Draw, { Type::Vec3f, Type::Vec2f });
		ib = new IndexBuffer(index, 6 * sprites.size(), Static | Draw, Type::UnsignedInt);		
		delete[] index;
	}

	void Render(Camera* cam, Transform* tran = nullptr) override
	{
		Material::Property* MVP_prop = mat->GetProperty("u_MVP");
		Material::Property* tex_prop = mat->GetProperty("u_texture");
		if (MVP_prop != nullptr)
			if (tran == nullptr)
				MVP_prop->SetData(cam->projMat * cam->viewMat);
			else
				MVP_prop->SetData(cam->projMat * cam->viewMat * tran->mat);
		if (tex_prop != nullptr)
			tex_prop->SetData(tex);

		mat->UpdateProperties();
		mat->GetShaderSelection()->Bind();
		va->Bind();
		ib->Bind();

		Renderer::DrawTriangles();
	}
};