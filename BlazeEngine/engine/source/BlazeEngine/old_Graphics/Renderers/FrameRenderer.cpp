#include "BlazeEngine/Graphics/Renderers/FrameRenderer.h"
#include "BlazeEngine/Graphics/GraphicsCore.h"
#include "BlazeEngine/Math/Math.h"

namespace Blaze::Graphics
{
	struct Vertex
	{		
		Vec4f fillColor;
		Vec4f borderColor;
		float cornerRadius;	
		float borderWidth;
		Vec2f size;
		Mat4f matrix;		
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
		va.EnableVertexAttribute(6);
		va.EnableVertexAttribute(7);
		va.EnableVertexAttribute(8);
		va.SetVertexAttributeFormat(0, GraphicsLibrary::VertexAttributeType::Float, 4, false, 0);
		va.SetVertexAttributeFormat(1, GraphicsLibrary::VertexAttributeType::Float, 4, false, 16);
		va.SetVertexAttributeFormat(2, GraphicsLibrary::VertexAttributeType::Float, 1, false, 32);
		va.SetVertexAttributeFormat(3, GraphicsLibrary::VertexAttributeType::Float, 1, false, 36);
		va.SetVertexAttributeFormat(4, GraphicsLibrary::VertexAttributeType::Float, 2, false, 40);		
		va.SetVertexAttributeFormat(5, GraphicsLibrary::VertexAttributeType::Float, 4, false, 48);
		va.SetVertexAttributeFormat(6, GraphicsLibrary::VertexAttributeType::Float, 4, false, 64);
		va.SetVertexAttributeFormat(7, GraphicsLibrary::VertexAttributeType::Float, 4, false, 80);
		va.SetVertexAttributeFormat(8, GraphicsLibrary::VertexAttributeType::Float, 4, false, 96);
		va.SetVertexAttributeBuffer(0, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(1, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(2, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(3, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(4, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(5, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(6, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(7, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(8, &renderer.GetBuffer(), sizeof(Vertex), 0);

		renderer.Allocate(1, sizeof(Vertex));		
	}
	FrameRenderer::~FrameRenderer()
	{		
	}
	Result FrameRenderer::SetProjectionMatrix(Mat4f mat)
	{
		proj = mat;
		return Result();
	}
	Result FrameRenderer::Render(Transform2D transform, const FrameGraphicsData& data) const
	{
		Render(transform.GetMatrix(data.size), data);		
		return Result();		
	}
	Result FrameRenderer::Render(Mat4f matrix, const FrameGraphicsData& data) const
	{ 
		CHECK_RESULT(CheckProgram());

		Vertex vertex;
		vertex.borderWidth = data.borderWidth;
		vertex.cornerRadius = data.cornerRadius;
		vertex.fillColor = (Vec4f)data.fillColor;
		vertex.borderColor = (Vec4f)data.borderColor;
		vertex.size = data.size;
		vertex.matrix = matrix.Transposed();

		Graphics::Renderer::SelectProgram(&program);
		Graphics::Renderer::SelectVertexArray(&va);

		program.SetUniform(0, proj);

		renderer.Render(ArrayView(&vertex, 1), [](uint vertexIndex, uint vertexCount) {
			Graphics::Renderer::RenderPrimitiveArray(Graphics::Renderer::PrimitiveType::Points, vertexIndex, vertexCount);
			});

		return Result();
	}
	Result FrameRenderer::SetShaderProgram(GraphicsLibrary::ShaderProgram&& program)
	{		
		this->program = std::move(program);

		return Result();
	}
	Result FrameRenderer::LoadDefaultShaderProgram() const
	{
		GraphicsLibrary::VertexShader vert;
		GraphicsLibrary::GeometryShader geom;
		GraphicsLibrary::FragmentShader frag;
		
		CHECK_RESULT(vert.Load("assets/default/shaders/frame/frame.vert"));
		CHECK_RESULT(geom.Load("assets/default/shaders/frame/frame.geom"));
		CHECK_RESULT(frag.Load("assets/default/shaders/frame/frame.frag"));

		if (program.GetState() != GraphicsLibrary::ShaderProgramState::Invalid)
			program = std::move(GraphicsLibrary::ShaderProgram());

		CHECK_RESULT(program.LinkShaders({ &vert, &geom, &frag }));

		return Result();
	}
	Result FrameRenderer::CheckProgram() const
	{
		switch (program.GetState())
		{
		case GraphicsLibrary::ShaderProgramState::Invalid:
			CHECK_RESULT(LoadDefaultShaderProgram());
		case GraphicsLibrary::ShaderProgramState::SuccesfullyLinked:
			return Result();
		case GraphicsLibrary::ShaderProgramState::UnsuccesfullyLinked:
			return BLAZE_ERROR_RESULT("Blaze Engine", "Trying to draw a frame but the shader program wasn't succesfully linked");
		default:
			return BLAZE_ERROR_RESULT("Blaze Engine", "Invalid ShaderProgramState value");
		}		
	}
}