#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

template<typename ... T>
struct VertexLayout
{
	using TemplateGroup = TemplateGroup<T...>;
};

template<typename _VertexLayout, typename _IndexType>
class UnpackedMeshData;

template<typename ... T, typename _IndexType>
class UnpackedMeshData<VertexLayout<T...>, _IndexType>
{	
	using AttributesTupleTypes = Blaze::TemplateGroup<T*...>;
public:
	using IndexType = _IndexType;
	using Vertex = Tuple<T...>;
private:
	Tuple<T*...> attributes;
	IndexType* indices;

	uint vertexCount;
	uint indexCount;

	template<typename T>
	inline void AllocateArrays(T& tuple, uint count)
	{
		delete[] tuple.value;
		tuple.value = new std::remove_pointer_t<typename T::TemplateGroup::First>[count];
		
		if constexpr (T::TemplateGroup::Size > 1)
			AllocateArrays(tuple.nextTuple, count);
	}
	template<typename T>
	inline void DeallocateArrays(T& tuple)
	{
		delete[] tuple.value;

		if constexpr (T::TemplateGroup::Size > 1)
			DeallocateArrays(tuple.nextTuple);
	}
	template<typename T>
	inline void SetNullptrArrays(T& tuple)
	{
		tuple.value = nullptr;

		if constexpr (T::TemplateGroup::Size > 1)
			SetNullptrArrays(tuple.nextTuple);
	}
	template<typename T1, typename T2>
	inline void SetValueArrays(T1& tuple, T2& vertex, uint index)
	{
		tuple.value[index] = vertex.value;

		if constexpr (T1::TemplateGroup::Size > 1)
			SetValueArrays(tuple.nextTuple, vertex.nextTuple, index);
	}
public:	
	UnpackedMeshData()
		: vertexCount(0), indexCount(0)
	{
		SetNullptrArrays(attributes);
	}
	~UnpackedMeshData()
	{
		DeallocateArrays(attributes);
	}

	inline void SetVertexCount(uint count)
	{
		vertexCount = count;		

		AllocateArrays(attributes, count);
	}
	inline void SetIndexCount(uint count)
	{
		indexCount = indexCount;
	}

	inline uint GetVertexCount() { return vertexCount; }
	inline uint GetIndexCount() { return indexCount; }

	inline void SetVertex(uint index, const Vertex& vertex)
	{
		SetValueArrays(attributes, vertex, index);
	}
	inline void SetIndex(uint index, IndexType value)
	{
		indices[index] = value;
	}
	
	template<size_t AttributeIndex>
	constexpr auto GetAttributePtr()
	{
		return attributes.GetValueByIndex<AttributeIndex>();
	}
	IndexType* GetIndicesPtr()
	{
		return indices;
	}
};

class App : public Application<App>
{
public:
	Console::LogList logList = Console::LogList({ 0, 0 }, { 150, 40 });
	Window window;

	Graphics::Program program;
	Graphics::VertexArray vertexArray;
	Graphics::ImmutableBuffer<true, false> vertexPositionBuffer;
	Graphics::ImmutableBuffer<true, false> vertexUVBuffer;
	Graphics::ImmutableBuffer<true, false> vertexNormalBuffer;
	Graphics::ImmutableBuffer<true, false> indexBuffer;

	Mat4f projMat = Math::PerspectiveMatrix<float>(90, 1, 0.1, 1000);
	Transform3D cameraTransform;
	Vec3f cameraRotation;
	int MVPuniform;

	Texture2D texture;

	App()
	{

	}

	static void WindowCloseEvent(Window* win)
	{
		App::Stop();
		TemplateGroup<int, float, double>::TypeAtIndex<1> a;
	}

	void Startup() override
	{
		window.SetPos(Vec2i(100, 100)); 
		window.SetSize(Vec2i(500, 500));

		window.ShowWindow(true);
		Renderer::SetTarget(window);
		Renderer::SetViewport({}, window.GetSize());

		Input::SetEventFunction<InputEvent::WindowClosed>(WindowCloseEvent);

		/*float positions[]{
			0.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f
		};
		vertexPositionBuffer.AllocateData(sizeof(positions), positions);
		float uvs[]{
			0.0f, 0.0f,
			1.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f
		};
		vertexUVBuffer.AllocateData(sizeof(uvs), uvs);
		float normals[]{
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f,
		};
		vertexNormalBuffer.AllocateData(sizeof(normals), normals);
		unsigned indices[]{
			0, 1, 2, 1, 2, 3
		};
		indexBuffer.AllocateData(sizeof(indices), indices);*/

		vertexArray.SetVertexAttributeData(0, vertexPositionBuffer, 12, 0, Graphics::AttributeType::Vec3f, false, 0);
		vertexArray.SetVertexAttributeData(1, vertexUVBuffer, 8, 0, Graphics::AttributeType::Vec2f, false, 0);
		vertexArray.SetVertexAttributeData(2, vertexNormalBuffer, 12, 0, Graphics::AttributeType::Vec3f, false, 0);
		vertexArray.SetIndexBuffer(indexBuffer);

		Graphics::Shader vertexShader(Graphics::ShaderType::VertexShader, "assets/shaders/vertex.glsl");
		Graphics::Shader fragmentShader(Graphics::ShaderType::FragmentShader, "assets/shaders/fragment.glsl");
		program.LinkShaders({ &vertexShader, &fragmentShader });

		int location = program.GetUniformLocation("u_texture");
		program.SetUniform(location, 0);
		MVPuniform = program.GetUniformLocation("u_MVP");

		texture.Load("assets/default/default.png");
		texture.Bind(0);

		//LoadPLY("assets/untitled.ply");
		UnpackedMeshData<VertexLayout<Vec3f, Vec2f, Vec3f>, unsigned> meshData;
		CreateIcosphere(meshData, 0);
		vertexPositionBuffer.AllocateData(meshData.GetVertexCount() * sizeof(Vec3f), meshData.GetAttributePtr<0>());
		vertexUVBuffer.AllocateData(meshData.GetVertexCount() * sizeof(Vec2f), meshData.GetAttributePtr<1>());
		vertexNormalBuffer.AllocateData(meshData.GetVertexCount() * sizeof(Vec3f), meshData.GetAttributePtr<2>());
		indexBuffer.AllocateData(meshData.GetIndexCount() * sizeof(decltype(meshData)::IndexType), meshData.GetIndicesPtr());

		
	}

	void Frame() override
	{
		Input::Update();

		MoveCamera();

		RenderScene();

		Time::Update();
	}

	void RenderScene()	
	{
		Renderer::ClearTarget(); 

		program.Use();
		Graphics::VertexArray::Bind(vertexArray);

		program.SetUniform(MVPuniform, projMat * cameraTransform.mat);
		Renderer::RenderTriangles(IndexType::UnsignedInt, 12);

		Graphics::VertexArray::Unbind();
		program.Unuse();

		Renderer::UpdateTarget();
	}

	void NewLog(const Log& log) override
	{
		logList.AddLog(log);
		logList.Refresh();
	}

	void MoveCamera()
	{
		if (Input::GetKeyState(Key::W).state == KeyState::Down)
			cameraTransform.pos += cameraTransform.rot * Vec3f(0, 0, 1) * Time::GetDeltaTime();
		if (Input::GetKeyState(Key::S).state == KeyState::Down)
			cameraTransform.pos += cameraTransform.rot * Vec3f(0, 0, -1) * Time::GetDeltaTime();
		if (Input::GetKeyState(Key::A).state == KeyState::Down)
			cameraTransform.pos += cameraTransform.rot * Vec3f(-1, 0, 0) * Time::GetDeltaTime();
		if (Input::GetKeyState(Key::D).state == KeyState::Down)
			cameraTransform.pos += cameraTransform.rot * Vec3f(1, 0, 0) * Time::GetDeltaTime();
		if (Input::GetKeyState(Key::LShift).state == KeyState::Down)
			cameraTransform.pos += Vec3f(0, -1, 0) * Time::GetDeltaTime();
		if (Input::GetKeyState(Key::Space).state == KeyState::Down)
			cameraTransform.pos += Vec3f(0, 1, 0) * Time::GetDeltaTime();

		if (Input::GetKeyState(Key::MouseMiddle).state == KeyState::Down)
		{
			cameraRotation += Vec3f(Input::GetMouseMovement().y, Input::GetMouseMovement().x, 0) * Time::GetDeltaTime() * 5;
			cameraTransform.rot = Quatf(AxisAnglef(Vec3f(0, 1, 0), cameraRotation.y)) * Quatf(AxisAnglef(Vec3f(1, 0, 0), cameraRotation.x));
		}

		cameraTransform.UpdateAsViewTransform();
	}

	void LoadPLY(const StringView& path)
	{
		StringStream ss;
		{
			ByteStream bs;
			File::Read(path, &bs);
			ss = std::move(bs);
		}

		enum class FileType {
			binary_little_endian,
			binary_big_endian,
			ASCII,
		} fileType;
		StringView version;
		StringView command;
		uint vertexCount = 0;
		uint faceCount = 0;
		uint indexCount = 0;
		int pos = 0;
		int posXOffset = -1;
		int posYOffset = -1;
		int posZOffset = -1;
		int uvXOffset = -1;
		int uvYOffset = -1;
		int normXOffset = -1;
		int normYOffset = -1;
		int normZOffset = -1;
		std::vector<float> temp;
		std::vector<Vec3f> vertices;
		std::vector<Vec2f> uvs;
		std::vector<Vec3f> normals;
		std::vector<uint> indices;

		command = ss.GetWord(); 
		if (command != "ply")
		{
			Logger::AddLog(LogType::Error, "LoadPLY", "Invalid ply file, doesnt start with ply");
			return;
		}

		command = ss.GetWord();
		if (command != "format")
		{
			Logger::AddLog(LogType::Error, "LoadPLY", "Invalid ply file, no format");
			return;
		}

		command = ss.GetWord();
		if (command == "ascii")
			fileType = FileType::ASCII;
		else if (command == "binary_little_endian")
			fileType = FileType::binary_little_endian;
		else if (command == "binary_big_endian")
			fileType = FileType::binary_big_endian;

		while ((command = ss.GetWord()) != "end_header")
		{
			if (ss.IsEnd())
			{
				Logger::AddLog(LogType::Error, "LoadPLY", "Invalid file");
				return;
			} 
			else if (command == "comment")
				Logger::AddLog(LogType::Message, "LoadPLY", "File message: " + ss.GetLine());
			else if (command == "element")
			{  
				command = ss.GetWord(); 
				if (command == "vertex")
					vertexCount = ss.Get<int>();
				else if (command == "face")
					faceCount = ss.Get<int>();
			}
			else if (command == "property")
			{
				command = ss.GetWord();
				if (command == "list")
				{

				}
				else if (command == "float")
				{
					command = ss.GetWord();
					if (command == "x") posXOffset = pos;
					else if (command == "y") posYOffset = pos;
					else if (command == "z") posZOffset = pos;
					else if (command == "s") uvXOffset = pos;
					else if (command == "t") uvYOffset = pos;
					else if (command == "nx") normXOffset = pos;
					else if (command == "ny") normYOffset = pos;
					else if (command == "nz") normZOffset = pos;
					else
					{
						Logger::AddLog(LogType::Warning, "LoadPLY", "Unsuported vertex attribute");
					}
					++pos;
				}
				else
					Logger::AddLog(LogType::Error, "LoadPLY", "Unsuported file type");				
			}
		}

		if (posXOffset == -1 || posYOffset == -1 || posZOffset == -1 || uvXOffset == -1 || uvYOffset == -1 || normXOffset == -1 || normYOffset == -1 || normZOffset == -1)
		{
			Logger::AddLog(LogType::Error, "LoadPLY", "Doesnt have needed attributes");
			return;
		}

		temp.resize(pos);		
		vertices.resize(vertexCount);
		uvs.resize(vertexCount);
		normals.resize(vertexCount);

		if (fileType == FileType::ASCII)
		{
			for (int i = 0; i < vertexCount; ++i)
			{				
				for (int j = 0; j < pos; ++j)				
					temp[j] = ss.Get<float>();									

				vertices[i].x = temp[posXOffset];
				vertices[i].y = temp[posYOffset];
				vertices[i].z = temp[posZOffset];
				uvs[i].x = temp[uvXOffset];
				uvs[i].y = temp[uvYOffset];
				normals[i].x = temp[normXOffset];
				normals[i].y = temp[normYOffset];
				normals[i].z = temp[normZOffset];
			} 

			uint ssOffset = ss.GetOffset();
			
			StringStream line;
			for (int i = 0; i < faceCount; ++i)
			{
				line = ss.GetLine();
				indexCount += (line.Get<int>() - 2) * 3;
			}

			ss.SetOffset(ssOffset);

			indices.resize(indexCount); 

			uint indexOffset = 0;
			for (int i = 0; i < faceCount; ++i)
			{
				int faceVertexCount = ss.Get<int>();

				indices[indexOffset] = ss.Get<uint>();
				indices[indexOffset + 1] = ss.Get<uint>();
				indices[indexOffset + 2] = ss.Get<uint>();
				for (int j = 1; j < faceVertexCount - 2; ++j)
				{
					indices[indexOffset + j * 3 + 0] = indices[indexOffset];
					indices[indexOffset + j * 3 + 1] = indices[indexOffset + j * 3 - 1];
					indices[indexOffset + j * 3 + 2] = ss.Get<uint>();
				}
				indexOffset += (faceVertexCount - 2) * 3;
			}			
		}		

		vertexPositionBuffer.AllocateData(sizeof(Vec3f) * vertexCount, vertices.data());
		vertexUVBuffer.AllocateData(sizeof(Vec2f) * vertexCount, uvs.data());
		vertexNormalBuffer.AllocateData(sizeof(Vec3f) * vertexCount, normals.data());
		indexBuffer.AllocateData(sizeof(uint) * indexCount, indices.data());		
	}		

		static constexpr uint a = 0b01010101010101010101010101010101 >> 32 - 2 * 0;
	template<typename _MeshType>
	void CreateIcosphere(_MeshType& mesh, uint subdivisions)
	{		
		uint32 a = 32 - 2 * subdivisions; 
		uint64 b = uint32(-0b11010101010101010101010101010101) >> a;
		uint vertexCount = 30 * b + 12;
		uint faceCount = 20 << (subdivisions * 2);		
		mesh.SetVertexCount(vertexCount);
		mesh.SetIndexCount(faceCount * 3);
		Vec3f* vertices = mesh.GetAttributePtr<0>();
		unsigned* indices = mesh.GetIndicesPtr();

		static constexpr float X = 0.525731112119133606;
		static constexpr float Z = 0.850650808352039932;
		static constexpr Vec3f startVertices[]{
			 Vec3f(-X, 0.0, Z), Vec3f(X, 0.0, Z),  Vec3f(-X, 0.0, -Z), Vec3f(X, 0.0, -Z),
			 Vec3f(0.0, Z, X),  Vec3f(0.0, Z, -X), Vec3f(0.0, -Z, X),  Vec3f(0.0, -Z, -X),
			 Vec3f(Z, X, 0.0),  Vec3f(-Z, X, 0.0), Vec3f(Z, -X, 0.0),  Vec3f(-Z, -X, 0.0)
		};
		static constexpr unsigned startIndices[]{
			0,4,1,0,9,4,9,5,4,4,5,8,4,8,1,
			8,10,1,8,3,10,5,3,8,5,2,3,2,7,3,
			7,10,3,7,6,10,7,11,6,11,0,6,0,1,6,
			6,1,10,9,0,11,9,11,2,9,2,5,7,2,11
		};

		memcpy(vertices, startVertices, sizeof(startVertices));
		memcpy(indices, startIndices, sizeof(startIndices));
	}
};

/*
Mesh: (immutable/mutable, static/dynamic, )
-vertex layout
-vertex count 
-vertex buffer

-index type
-index count
-index buffer

-used primitives <point/line/triangle>
-mesh use		 <draw, stream, copy>
*/

/*

Sprite	

Button

Text
-character per vertexMrMr

DropBox
*/