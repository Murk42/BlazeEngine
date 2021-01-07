#include "OldMesh.h"
#include "BlazeEngine/Utilities/File.h"
#include "BlazeEngine/DataStructures/StringView.h"
#include "BlazeEngine/DataStructures/StringStream.h"
#include "BlazeEngine/DataStructures/Vector.h"

namespace Blaze
{	
	OldMesh::OldMesh() : 
		vertex(nullptr),
		index(nullptr), 
		materialIndexCounts(nullptr), materialCount(0)
	{
	}
	OldMesh::OldMesh(const OldMesh& m)
		: vl(m.vl), vb(m.vb), ib(m.ib)
	{
		vertex = (float*)new uint8[m.vb.GetSize()];
		memcpy(vertex, m.vertex, m.vb.GetSize());

		index = (void*)new uint8[m.ib.GetSize()];
		memcpy(index, m.index, m.ib.GetSize());

		materialIndexCounts = new unsigned[m.materialCount];
		memcpy(materialIndexCounts, m.materialIndexCounts, sizeof(unsigned) * m.materialCount);
		materialCount = m.materialCount;
	}
	OldMesh::OldMesh(OldMesh&& m) :
		vertex(std::exchange(m.vertex, nullptr)),
		index(std::exchange(m.index, nullptr)),
		materialIndexCounts(std::exchange(m.materialIndexCounts, nullptr)), materialCount(m.materialCount),
		vl(std::move(m.vl)), vb(std::move(m.vb)), ib(std::move(m.ib))
	{

	}
	OldMesh::OldMesh(const String& path)
		: OldMesh()
	{
		Load(path);
	}
	OldMesh::~OldMesh()
	{
		if (vertex != nullptr) delete[] vertex;
		if (index != nullptr) delete[] index;
	}

	void OldMesh::SetVertexData(float* v, bool copy, unsigned count, const std::vector<Type> layout)
	{
		if (vertex != nullptr) delete[] vertex;

		int stride = 0;
		for (auto& t : layout) stride += SizeOf(t);

		if (copy)
		{
			vertex = new float[stride * count / sizeof(float)];
			memcpy(vertex, v, stride * count);
		}
		else
			vertex = v;

 		vl.SetLayout(layout);
		vl.BindVertexBuffer(&vb);
		vl.BindIndexBuffer(&ib);
		vb.AllocateData(vertex, count * stride, Dynamic | Draw);
	}
	void OldMesh::SetIndexData(void* i, bool copy, unsigned count, Type type)
	{
		if (index != nullptr) delete[] index;

		if (copy)
		{
			index = new byte[SizeOf(type) * count];
			memcpy(index, i, SizeOf(type) * count);
		}
		else
			index = i;

		vl.Bind();
		ib.AllocateData(index, count, Static | Draw, type);
	}

	void OldMesh::SetMaterialData(unsigned* i, bool copy, unsigned count)
	{
		if (materialIndexCounts != nullptr) delete[] materialIndexCounts;

		if (copy)
		{
			materialIndexCounts = new unsigned[count];
			memcpy(materialIndexCounts, i, count * sizeof(unsigned));
		}
		else
			materialIndexCounts = i;

		materialCount = count;
	}

	enum OldMeshFileType
	{
		BMO,
		OBJ,
	};

	bool OldMesh::Load(const String& path)
	{
		ByteStream bs;
		if (!File::Read(path, &bs))
			return false;

		OldMeshFileType fileType;

		int i = path.Find('.');
		String extension = String((const char*)path.Ptr() + i + 1);
		if (extension == "bmo")
			LoadBMO(bs);
		else if (extension == "obj")
			LoadOBJ(bs);
		else
			return false;
		
		return true;
	}
	bool OldMesh::Save(const String& path)
	{		
		ByteStream bs;		
		OldMeshFileType fileType;

		int i = path.Find('.');
		String extension = String((const char*)path.Ptr() + i + 1);
		if (extension == "bmo")
			SaveBMO(bs);
		else
			return false;

		return File::Write(path, true, &bs);		
	}

	void OldMesh::operator=(const OldMesh& m)
	{
		delete[] vertex;
		delete[] index;
		delete[] materialIndexCounts;

		vertex = (float*)new uint8[m.vb.GetSize()];
		memcpy(vertex, m.vertex, m.vb.GetSize());

		index = (void*)new uint8[m.ib.GetSize()];
		memcpy(index, m.index, m.ib.GetSize());

		materialIndexCounts = new unsigned[m.materialCount];
		memcpy(materialIndexCounts, m.materialIndexCounts, sizeof(unsigned) * m.materialCount);

		materialCount = m.materialCount;

		vl = m.vl;
		vb = m.vb;
		ib = m.ib;
	}
	void OldMesh::operator=(OldMesh&& m)
	{
		delete[] vertex;
		delete[] index;
		delete[] materialIndexCounts;

		vertex = std::exchange(m.vertex, nullptr); 		
		index = std::exchange(m.index, nullptr);		
		materialIndexCounts = std::exchange(m.materialIndexCounts, nullptr);
		materialCount = m.materialCount;

		vl = std::move(m.vl);
		vb = std::move(m.vb);
		ib = std::move(m.ib);
	}

	bool OldMesh::LoadBMO(ByteStream& bs)
	{
		uint vertexCount, indexCount, materialCount;		
		Type indexType;
		
		size_t vertexLayoutCount;
		vertexLayoutCount = bs.Get<uint32>();
		std::vector<Type> vertexLayout(vertexLayoutCount);
		bs.Get(vertexLayout.data(), sizeof(Type) * vertexLayoutCount);

		size_t stride = vl.GetStride();

		vertexCount = bs.Get<uint32>();		
		vertex = (float*)new uint8[stride * vertexCount];
		bs.Get(vertex, stride * vertexCount);
		
		indexType = (Type)bs.Get<uint32>();
		indexCount = bs.Get<uint32>();		
		index = new uint[indexCount];
		bs.Get(index, sizeof(uint) * indexCount);

		materialCount = bs.Get<uint32>();		
		materialIndexCounts = new uint[materialCount];
		bs.Get(materialIndexCounts, sizeof(uint) * materialCount);

		vl.SetLayout(vertexLayout);
		vl.BindVertexBuffer(&vb);
		vl.BindIndexBuffer(&ib);
		vb.AllocateData(vertex, stride * vertexCount, Static | Draw);
		ib.AllocateData(index, indexCount, Static | Draw, indexType);
		return true;
	}
	bool OldMesh::LoadOBJ(ByteStream& bs)
	{
		auto Get = [](StringView& s, char sep = ' ')
		{
			int i = s.Find({ sep,'\n', '\r' });
			StringView out = StringView((char*)s.Ptr(), (size_t)(i == -1 ? s.Size() : i));
			s = StringView((char*)s.Ptr() + out.Size() + 1, (size_t)(i == -1 ? 0 : s.Size() - out.Size() - 1));
			return out;
		};
		auto GetF = [](StringView& s)
		{
			char* ptr;
			float val = strtof(s.Ptr(), &ptr);
			s = StringView(ptr, s.Size() - (ptr - s.Ptr()));
			return val;
		};
		auto GetI = [](StringView& s)
		{
			char* ptr;
			int val = strtol(s.Ptr(), &ptr, 10);
			s = StringView(ptr, s.Size() - (ptr - s.Ptr()));
			return val;
		};
		StringStream ss = std::move(bs);		

		std::vector<Vec3f> pos;
		std::vector<Vec3f> norm;
		std::vector<Vec2f> uv;
		std::vector<std::tuple<Vec3i, Vec3i, Vec3i>> tri;		
				
		float x, y, z;		
		StringView line, op, val;
		while (ss.GetOffset() != ss.GetSize())
		{
			ss.GetLine(line);			
			op = Get(line);

			if (op == "v")
			{	
				x = GetF(line);
				y = GetF(line);
				z = GetF(line);	
				pos.emplace_back(x, y, z); 
			}
			else if (op == "vn")
			{
				x = GetF(line);
				y = GetF(line);
				z = GetF(line);
				norm.emplace_back(x, y, z);
			}
			else if (op == "vt")
			{
				x = GetF(line);
				y = GetF(line);				
				uv.emplace_back(x, y);
			}
			else if (op == "f")
			{
				std::vector<Vec3i> indicies;
				indicies.reserve(3);

				while (line.Size() != line.Count(' '))
				{
					int indexCount;
					StringView si, si1, si2, si3;
					si = Get(line, ' ');
					indexCount = si.Count('/');
					switch (indexCount)
					{
					case 0:
						si1 = si;
						break;
					case 1:
						si1 = Get(si, '/');
						si2 = si;
						break;
					case 2:
						si1 = Get(si, '/');
						si2 = Get(si, '/');
						si3 = si;
						break;
					}

					indicies.emplace_back(GetI(si1), si2.Size() > 0 ? GetI(si2) : -1, si3.Size() > 0 ? GetI(si3) : -1);
				}

				for (int i = 0; i < indicies.size() - 2; i++)				
					tri.emplace_back(indicies[0], indicies[i + 1], indicies[i + 2]);				
			}
		}

		size_t stride = 3 + 3 * !norm.empty() + 2 * !uv.empty();
		this->vertex = new float[stride * tri.size() * 3];
		this->index = new unsigned[tri.size() * 3];
		this->materialIndexCounts = new unsigned(tri.size() * 3);

		float* ptr = this->vertex;
		for (int i = 0; i < tri.size() * 3; i++)
		{
			Vec3i& index = ((Vec3i*)tri.data())[i];
			index = index - Vec3i(1, 1, 1);
			*(Vec3f*)ptr = pos[index.x];
			ptr += 3;
			if (!uv.empty())
			{
				*(Vec2f*)ptr = uv[index.y];				
				ptr += 2;
			}
			if (!norm.empty())
			{
				*(Vec3f*)ptr = norm[index.z];
				ptr += 3;
			}
			((unsigned*)this->index)[i] = i;
		}

		std::vector<Type> vertexLayout = { Type::Vec3f };
		if (!uv.empty()) vertexLayout.emplace_back(Type::Vec2f);
		if (!norm.empty()) vertexLayout.emplace_back(Type::Vec3f);
		vl.SetLayout(vertexLayout);
		vl.BindVertexBuffer(&vb);
		vl.BindIndexBuffer(&ib);
		vb.AllocateData(vertex, stride * sizeof(float) * tri.size() * 3, Static | Draw);
		ib.AllocateData(index, tri.size() * 3, Static | Draw, Type::UnsignedInt);		
		this->materialCount = 1;				

		return true;
	}
	bool OldMesh::SaveBMO(ByteStream& bs)
	{		
		bs.Set<uint32>(vl.GetLayout().size());
		bs.Set(vl.GetLayout().data(), sizeof(Type) * vl.GetLayout().size());
		size_t stride = vl.GetStride();

		bs.Set<uint32>(vb.GetSize() / stride);
		bs.Set(vertex, vb.GetSize());

		bs.Set<uint32>((uint32)ib.GetIndexType());
		bs.Set<uint32>(ib.GetIndexCount());
		bs.Set(index, ib.GetSize());

		bs.Set<uint32>(materialCount);
		bs.Set(materialIndexCounts, sizeof(uint) * materialCount);
		return true;		
	}
}