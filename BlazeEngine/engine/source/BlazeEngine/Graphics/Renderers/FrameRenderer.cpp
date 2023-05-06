#include "BlazeEngine/Graphics/Renderers/FrameRenderer.h"
#include "BlazeEngine/Graphics/GraphicsCore.h"

namespace Blaze::Graphics
{
	struct Vertex
	{
		Vec3f pos;
		Vec4f rect;
		Vec4f fillColor;
		Vec4f borderColor;
		float cornerRadius;
		float borderWidth;
	};
	static constexpr size_t bufferSize = sizeof(Vertex) * 6;
	FrameRenderer::FrameRenderer()
	{		
		va.EnableVertexAttribute(0);
		va.EnableVertexAttribute(1);
		va.EnableVertexAttribute(2);
		va.EnableVertexAttribute(3);
		va.EnableVertexAttribute(4);
		va.EnableVertexAttribute(5);
		va.SetVertexAttributeFormat(0, Graphics::Core::VertexAttributeType::Float, 3, false, 0);
		va.SetVertexAttributeFormat(1, Graphics::Core::VertexAttributeType::Float, 4, false, 12);
		va.SetVertexAttributeFormat(2, Graphics::Core::VertexAttributeType::Float, 4, false, 28);
		va.SetVertexAttributeFormat(3, Graphics::Core::VertexAttributeType::Float, 4, false, 44);
		va.SetVertexAttributeFormat(4, Graphics::Core::VertexAttributeType::Float, 1, false, 60);
		va.SetVertexAttributeFormat(5, Graphics::Core::VertexAttributeType::Float, 1, false, 64);
		//va.SetVertexAttributeBuffer(0, &renderer.GetBuffer(), sizeof(Vertex), 0);
		//va.SetVertexAttributeBuffer(1, &renderer.GetBuffer(), sizeof(Vertex), 0);
		//va.SetVertexAttributeBuffer(2, &renderer.GetBuffer(), sizeof(Vertex), 0);
		//va.SetVertexAttributeBuffer(3, &renderer.GetBuffer(), sizeof(Vertex), 0);
		//va.SetVertexAttributeBuffer(4, &renderer.GetBuffer(), sizeof(Vertex), 0);
		//va.SetVertexAttributeBuffer(5, &renderer.GetBuffer(), sizeof(Vertex), 0);				

		//renderer.Allocate(4, sizeof(Vertex) * 6);		
	}
	FrameRenderer::~FrameRenderer()
	{		
	}
	Result FrameRenderer::Render(Vec2f pos, Vec2f size, ColorRGBAf fillColor, ColorRGBAf borderColor, float borderWidth, float cornerRadius)
	{
		if (program.GetState() == Graphics::Core::ShaderProgramState::Invalid)
			CHECK_RESULT(LoadDefaultShaders());
		Vertex vertices[6];

		Vec2f positions[4];
		positions[0] = { pos.x, pos.y };
		positions[1] = { pos.x + size.x, pos.y };
		positions[2] = { pos.x, pos.y + size.y };
		positions[3] = { pos.x + size.x, pos.y + size.y };

		vertices[0].pos = Vec3f(positions[0], 0);
		vertices[1].pos = Vec3f(positions[2], 0);
		vertices[2].pos = Vec3f(positions[1], 0);
		vertices[3].pos = Vec3f(positions[1], 0);
		vertices[4].pos = Vec3f(positions[2], 0);
		vertices[5].pos = Vec3f(positions[3], 0);

		for (uint i = 0; i < 6; ++i)
		{			
			vertices[i].rect = { pos, size };
			vertices[i].borderWidth = borderWidth;
			vertices[i].cornerRadius = cornerRadius;
			vertices[i].fillColor = (Vec4f)fillColor;
			vertices[i].borderColor = (Vec4f)borderColor;
		}		

		Graphics::Core::SelectProgram(&program);
		Graphics::Core::SelectVertexArray(&va);

		program.SetUniform(0, proj);		

		//renderer.Render(vertices, 6, [](uint vertexIndex, uint vertexCount) {
		//	Graphics::Core::RenderPrimitiveArray(Graphics::Core::PrimitiveType::Triangles, vertexIndex, vertexCount);
		//	});

		return Result();
	}
	void FrameRenderer::SetProjectionMatrix(Mat4f mat)
	{
		proj = mat;
	}
	Result FrameRenderer::SetShaders(std::initializer_list<Graphics::Core::Shader*> shaders)
	{		
		if (program.GetState() != Graphics::Core::ShaderProgramState::Invalid)
			program = std::move(Graphics::Core::ShaderProgram());
		
		return ADD_STACK_FRAME(program.LinkShaders(shaders));
	}
	Result FrameRenderer::LoadDefaultShaders()
	{
		Graphics::Core::VertexShader vert;
		Graphics::Core::FragmentShader frag;

		
		CHECK_RESULT(vert.Load("assets/default/shaders/ui_frame.vert"));
		CHECK_RESULT(frag.Load("assets/default/shaders/ui_frame.frag"));

		if (program.GetState() != Graphics::Core::ShaderProgramState::Invalid)
			program = std::move(Graphics::Core::ShaderProgram());

		CHECK_RESULT(program.LinkShaders({ &vert, &frag }));

		return Result();
	}
}