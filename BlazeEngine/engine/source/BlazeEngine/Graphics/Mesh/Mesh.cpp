#include "BlazeEngine/Graphics/Mesh/Mesh.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/Graphics/Renderer.h"
#include "BlazeEngine/Utilities/File.h"
#include "BlazeEngine/DataStructures/StringStream.h"

namespace Blaze
{
	MeshData::MeshData()
		: vertexBufferSize(0), vertexBuffer(nullptr), indexBufferSize(0), indexBuffer(nullptr)
	{
	}
	MeshData::~MeshData()
	{
		free(vertexBuffer);
		free(indexBuffer);
	}
	void MeshData::Load(StringView path, MeshFileType fileType)
	{
		File file = File(path, FileOpenMode::Read);
		String fileData;

		{
			Buffer buffer;
			buffer.Allocate(file.Size());
			file.Read(buffer);

			fileData = String((char*)buffer.Ptr(), buffer.Size());
		}

		std::vector<Vec3f> positionsRaw;
		std::vector<Vec3f> normalsRaw;
		std::vector<Vec2f> uvsRaw;
		std::vector<Vec3i> indicesRaw;

		//Read file  
		{
			StringStream ss = StringStream(fileData);
			while (!ss.IsEnd())
			{
				StringView command = ss.GetWord();

				if (command == "v")
				{
					Vec3f& positions = positionsRaw.emplace_back();
					if (!String::ConvertTo<float>(ss.GetWord(), positions.x)) throw;
					if (!String::ConvertTo<float>(ss.GetWord(), positions.y)) throw;
					if (!String::ConvertTo<float>(ss.GetWord(), positions.z)) throw;
				}
				else if (command == "vn")
				{
					Vec3f& normal = normalsRaw.emplace_back();
					if (!String::ConvertTo<float>(ss.GetWord(), normal.x)) throw;
					if (!String::ConvertTo<float>(ss.GetWord(), normal.y)) throw;
					if (!String::ConvertTo<float>(ss.GetWord(), normal.z)) throw;
				}
				else if (command == "vt")
				{
					Vec2f& uv = uvsRaw.emplace_back();
					if (!String::ConvertTo<float>(ss.GetWord(), uv.x)) throw;
					if (!String::ConvertTo<float>(ss.GetWord(), uv.y)) throw;
				}
				else if (command == "f")
				{
					Vec3i indices[3];
					int indicesSet = 0;

					while (!ss.IsEndOfLine())
					{
						StringStream index = ss.GetWord();
						size_t count = index.GetAsStringView().Count('/');
						if (count == 0)
							String::ConvertTo<int>(index.GetAsStringView(), indices[indicesSet].x);
						else
						{
							String::ConvertTo<int>(index.GetUntil('/'), indices[indicesSet].x);
							index.Skip(1);
							if (count > 0)
							{
								String::ConvertTo<int>(index.GetUntil('/'), indices[indicesSet].y);
								index.Skip(1);

								if (count > 1)
								{
									String::ConvertTo<int>(index.GetUntil('/'), indices[indicesSet].z);
									index.Skip(1);
								}
							}
						}

						if (indicesSet == 2)
						{
							indicesSet = 1;
							indicesRaw.emplace_back(indices[0]);
							indicesRaw.emplace_back(indices[1]);
							indicesRaw.emplace_back(indices[2]);
							indices[1] = indices[2];
						}
						++indicesSet;
					}
				}
				else
					ss.SkipUntil('\n');
			}
		}		

		size_t stride = 0;

		if (positionsRaw.size() > 0)
		{
			vertexBufferSize += indicesRaw.size() * sizeof(Vec3f);
			stride += sizeof(Vec3f);
		}
		if (normalsRaw.size() > 0)
		{
			vertexBufferSize += indicesRaw.size() * sizeof(Vec3f);
			stride += sizeof(Vec3f);
		}
		if (uvsRaw.size() > 0)
		{
			vertexBufferSize += indicesRaw.size() * sizeof(Vec2f);
			stride += sizeof(Vec2f);
		}

		free(vertexBuffer);
		vertexBuffer = malloc(vertexBufferSize);

		size_t attributeOffset = 0;

		if (positionsRaw.size() > 0)
		{
			size_t offset = attributeOffset;
			for (size_t i = 0; i < indicesRaw.size(); ++i)
			{
				size_t j = indicesRaw[i].x;

				if (j < 0) 
					j += positionsRaw.size(); 
				else 
					--j;

				memcpy((char*)vertexBuffer + offset, &positionsRaw[j], sizeof(Vec3f));
				offset += stride;
			}
			attributeOffset += sizeof(Vec3f);
		}
		if (normalsRaw.size() > 0)
		{
			size_t offset = attributeOffset;
			for (size_t i = 0; i < indicesRaw.size(); ++i)
			{
				size_t j = indicesRaw[i].z;

				if (j < 0) 
					j += normalsRaw.size(); 
				else 
					--j;

				memcpy((char*)vertexBuffer + offset, &normalsRaw[j], sizeof(Vec3f));
				offset += stride;
			}
			attributeOffset += sizeof(Vec3f);
		}
		if (uvsRaw.size() > 0)
		{
			size_t offset = attributeOffset;
			for (size_t i = 0; i < indicesRaw.size(); ++i)
			{
				size_t j = indicesRaw[i].y;

				if (j < 0) 
					j += uvsRaw.size(); 
				else 
					--j;

				memcpy((char*)vertexBuffer + offset, &uvsRaw[j], sizeof(Vec2f));
				offset += stride;
			}
			attributeOffset += sizeof(Vec2f);
		}

		indexBufferSize = sizeof(unsigned) * indicesRaw.size();
		free(indexBuffer);
		indexBuffer = malloc(indexBufferSize);

		size_t offset = 0;
		for (unsigned i = 0; i < indicesRaw.size(); ++i)
		{
			memcpy((char*)indexBuffer + offset, &i, sizeof(unsigned));
			offset += sizeof(unsigned);
		}
	}
}