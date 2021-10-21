#include "Planet.h"
#include "App.h"

PlanetMesh::PlanetMesh()
	: vertexCount(-1)
{
	va.SetVertexAttributeData(0, VertexAttribute(&vb, sizeof(Vertex), 0, VertexAttributeType::Float, 3, false, 0));	
	va.SetVertexAttributeData(1, VertexAttribute(&vb, sizeof(Vertex), 0, VertexAttributeType::Float, 3, false, sizeof(float) * 3));
}

void PlanetMesh::Render()
{
	VertexArray::Bind(va);
	Renderer::RenderTriangleArray(vertexCount, 0);
}

void PlanetMesh::UpdateMesh(BufferView buffer)
{
	vertexCount = buffer.Size() / sizeof(Vertex);
	vb.AllocateDynamicStorage(buffer, GraphicsBufferDynamicStorageHint::DynamicDraw);
}



PlanetChunk::~PlanetChunk()
{
	delete[] subChunks;
}

void PlanetChunk::Update()
{	
	Vec3f direction = (p1 + p2 + p3) / 3 + planet->transform.pos;
	float distance = direction.Lenght();		
	direction = direction / distance;

	bool isLeaf = false;		
	float dot;
	
	switch (level)
	{
	case 0: 
		dot = Math::DotProduct<float>((n1 + n2 + n3).Normalised(), -direction);
		if (dot > -0.5f)			
			isLeaf = distance > 10000; 
		else
			isLeaf = true;
		break;
	case 1: isLeaf = distance > 3000; break;
	case 2: isLeaf = distance > 300; break;
	case 3: isLeaf = distance > 100; break;
	case 4: isLeaf = distance > 50; break;
	case 5: isLeaf = distance > 10; break;
	case 6: isLeaf = distance > 0; break;
	}		

	if (subChunks != nullptr)	
		if (isLeaf)
		{
			delete[] subChunks;
			subChunks = nullptr;
		}
		else
		{
			subChunks[0].Update();
			subChunks[1].Update();
			subChunks[2].Update();
			subChunks[3].Update();
			return;
		}	
	else
		if (isLeaf == false)
		{
			/*			
			      1
			     / \
			    /   \
			   4 - - 6
			  / \   / \
			 /   \ /   \
			2 - - 5 - - 3			
			*/

			Console::Write("Creating chunks level " + String::Convert(level + 1) + "\n");

			subChunks = new PlanetChunk[4];
			Vec3f n4 = (n1 + n2) / 2;
			Vec3f n5 = (n2 + n3) / 2;
			Vec3f n6 = (n3 + n1) / 2;
			n4.Normalise();
			n5.Normalise();
			n6.Normalise();			

			subChunks[0].n1 = n1;
			subChunks[0].n2 = n4;
			subChunks[0].n3 = n6;
			subChunks[0].planet = planet;
			subChunks[0].level = level + 1;
			subChunks[0].subChunks = nullptr;
			subChunks[0].CalculateMesh();

			subChunks[1].n1 = n4;
			subChunks[1].n2 = n2;
			subChunks[1].n3 = n5;
			subChunks[1].planet = planet;
			subChunks[1].level = level + 1;
			subChunks[1].subChunks = nullptr;
			subChunks[1].CalculateMesh();

			subChunks[2].n1 = n4;
			subChunks[2].n2 = n5;
			subChunks[2].n3 = n6;
			subChunks[2].planet = planet;
			subChunks[2].level = level + 1;
			subChunks[2].subChunks = nullptr;
			subChunks[2].CalculateMesh();

			subChunks[3].n1 = n6;
			subChunks[3].n2 = n5;
			subChunks[3].n3 = n3;
			subChunks[3].planet = planet;
			subChunks[3].level = level + 1;
			subChunks[3].subChunks = nullptr;
			subChunks[3].CalculateMesh();

			subChunks[0].Update();
			subChunks[1].Update();
			subChunks[2].Update();
			subChunks[3].Update();
			return;
		}		

	planet->vertices.emplace_back(p1, normal);
	planet->vertices.emplace_back(p2, normal);
	planet->vertices.emplace_back(p3, normal);
}

void PlanetChunk::CalculateMesh()
{		
	p1 = n1 * (planet->size + Random::Simplex(n1) * 5.0f);
	p2 = n2 * (planet->size + Random::Simplex(n2) * 5.0f);
	p3 = n3 * (planet->size + Random::Simplex(n3) * 5.0f);
	normal = Math::CrossProduct(p2 - p1, p3 - p1).Normalised();
}



void Planet::UpdateChunks(const Transform3D& cameraTransform)
{	
	vertices.clear();

	for (size_t i = 0; i < rootChunkCount; ++i)
		chunks[i].Update();
	
	mesh.UpdateMesh(BufferView(vertices.data(), vertices.size() * sizeof(PlanetMesh::Vertex)));
}

constexpr Vec3f icosphereVertices[]{
	Vec3f(0.000000, 0.000000,-1.000000),
Vec3f(0.723600,-0.525720,-0.447215),
Vec3f(-0.276385,-0.850640,-0.447215),
Vec3f(0.723600,-0.525720,-0.447215),
Vec3f(0.000000, 0.000000,-1.000000),
Vec3f(0.723600, 0.525720,-0.447215),
Vec3f(0.000000, 0.000000,-1.000000),
Vec3f(-0.276385,-0.850640,-0.447215),
Vec3f(-0.894425, 0.000000,-0.447215),
Vec3f(0.000000, 0.000000,-1.000000),
Vec3f(-0.894425, 0.000000,-0.447215),
Vec3f(-0.276385, 0.850640,-0.447215),
Vec3f(0.000000, 0.000000,-1.000000),
Vec3f(-0.276385, 0.850640,-0.447215),
Vec3f(0.723600, 0.525720,-0.447215),
Vec3f(0.723600,-0.525720,-0.447215),
Vec3f(0.723600, 0.525720,-0.447215),
Vec3f(0.894425, 0.000000, 0.447215),
Vec3f(-0.276385,-0.850640,-0.447215),
Vec3f(0.723600,-0.525720,-0.447215),
Vec3f(0.276385,-0.850640, 0.447215),
Vec3f(-0.894425, 0.000000,-0.447215),
Vec3f(-0.276385,-0.850640,-0.447215),
Vec3f(-0.723600,-0.525720, 0.447215),
Vec3f(-0.276385, 0.850640,-0.447215),
Vec3f(-0.894425, 0.000000,-0.447215),
Vec3f(-0.723600, 0.525720, 0.447215),
Vec3f(0.723600, 0.525720,-0.447215),
Vec3f(-0.276385, 0.850640,-0.447215),
Vec3f(0.276385, 0.850640, 0.447215),
Vec3f(0.723600,-0.525720,-0.447215),
Vec3f(0.894425, 0.000000, 0.447215),
Vec3f(0.276385,-0.850640, 0.447215),
Vec3f(-0.276385,-0.850640,-0.447215),
Vec3f(0.276385,-0.850640, 0.447215),
Vec3f(-0.723600,-0.525720, 0.447215),
Vec3f(-0.894425, 0.000000,-0.447215),
Vec3f(-0.723600,-0.525720, 0.447215),
Vec3f(-0.723600, 0.525720, 0.447215),
Vec3f(-0.276385, 0.850640,-0.447215),
Vec3f(-0.723600, 0.525720, 0.447215),
Vec3f(0.276385, 0.850640, 0.447215),
Vec3f(0.723600, 0.525720,-0.447215),
Vec3f(0.276385, 0.850640, 0.447215),
Vec3f(0.894425, 0.000000, 0.447215),
Vec3f(0.276385,-0.850640, 0.447215),
Vec3f(0.894425, 0.000000, 0.447215),
Vec3f(0.000000, 0.000000, 1.000000),
Vec3f(-0.723600,-0.525720, 0.447215),
Vec3f(0.276385,-0.850640, 0.447215),
Vec3f(0.000000, 0.000000, 1.000000),
Vec3f(-0.723600, 0.525720, 0.447215),
Vec3f(-0.723600,-0.525720, 0.447215),
Vec3f(0.000000, 0.000000, 1.000000),
Vec3f(0.276385, 0.850640, 0.447215),
Vec3f(-0.723600, 0.525720, 0.447215),
Vec3f(0.000000, 0.000000, 1.000000),
Vec3f(0.894425, 0.000000, 0.447215),
Vec3f(0.276385, 0.850640, 0.447215),
Vec3f(0.000000, 0.000000, 1.000000)
};

void Planet::Initialize(float size, Vec3f pos, Star* star)
{
	this->size = size;
	this->star = star;
	transform.pos = pos;	

	for (size_t i = 0; i < rootChunkCount; ++i)
	{
		chunks[i].planet = this;
		chunks[i].level = 0;
		chunks[i].subChunks = nullptr;
		chunks[i].n1 = icosphereVertices[i * 3 + 0];
		chunks[i].n2 = icosphereVertices[i * 3 + 1];
		chunks[i].n3 = icosphereVertices[i * 3 + 2];
		chunks[i].CalculateMesh();
	}	
}

void Planet::UpdateTransform(Vec3f cameraDelta)
{
	transform.pos -= cameraDelta;
	transform.Update();
}

void Planet::Render(const Mat4f& VP3D)
{
	app->planetMaterial.starPos = star->transform.pos;
	app->planetMaterial.model = transform.mat;
	app->planetMaterial.MVP = VP3D * transform.mat;	
	app->planetMaterial.UpdateProperties();
	app->planetProgram.Use();
	
	//Renderer::RenderWireframe(true);
	mesh.Render();		
	//Renderer::RenderWireframe(false );
}