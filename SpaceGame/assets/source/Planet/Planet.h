#pragma once
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;
using namespace Graphics::Core;

#include "Star/Star.h"

struct PlanetMaterial : Material {
	Property<Vec3f> starPos = "u_starPos";
	Property<Mat4f> model = "u_model";
	Property<Mat4f> MVP = "u_MVP";
};

class PlanetMesh : public Mesh
{
public:
	struct Vertex 
	{
		Vec3f pos;
		Vec3f normal;

		Vertex() = default;
		Vertex(Vec3f pos, Vec3f normal) : pos(pos), normal(normal) { }
	};

	GraphicsBuffer vb;	
	size_t vertexCount;

	PlanetMesh();

	void Render() override;

	void UpdateMesh(BufferView buffer);
};

class Planet;

class PlanetChunk
{
public:		
	Vec3f n1, n2, n3;
	Vec3f p1, p2, p3;
	Vec3f normal;
	uint level;
	Planet* planet;
	PlanetChunk* subChunks;	

	~PlanetChunk();

	void Update();

	void CalculateMesh();
};

struct Planet 
{			
	std::vector<PlanetMesh::Vertex> vertices;	

	static constexpr size_t rootChunkCount = 20;
	PlanetChunk chunks[rootChunkCount];
	PlanetMesh mesh;	
	Star* star;
	Transform3D transform;
	float size;

	void UpdateChunks(const Transform3D& cameraTransform);

	void Initialize(float size, Vec3f pos, Star* star);
	void UpdateTransform(Vec3f cameraDelta);
	void Render(const Mat4f& VP3D);
};