#pragma once
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;
using namespace Blaze::Graphics;
using namespace Blaze::Graphics::Core;

struct StarMaterial : Material
{
	Property<float> size = "u_size";	
	Property<Mat4f> MVP = "u_MVP";
};

struct StarMesh : Mesh
{
	GraphicsBuffer vb;
	GraphicsBuffer ib;
	size_t indexCount;

	StarMesh();

	void Render() override;
};

struct Star
{	
	StarMesh mesh;
	Transform3D transform;	
	size_t size;

	void Initialize(Vec3f pos, size_t size);
	void UpdateTransform(Vec3f cameraDelta);
	void Render(const Mat4f& VP3D);
};