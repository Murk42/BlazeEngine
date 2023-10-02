#include "BlazeEngine/Graphics/Renderers/ImageRenderer.h"
#include "BlazeEngine/Graphics/GraphicsCore.h"

namespace Blaze::Graphics
{
	ImageRenderer::ImageRenderer()
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
		va.EnableVertexAttribute(9);
		va.EnableVertexAttribute(10);
		va.SetVertexAttributeFormat(0, GraphicsLibrary::VertexAttributeType::Float, 4, false, sizeof(float) * 0);
		va.SetVertexAttributeFormat(1, GraphicsLibrary::VertexAttributeType::Float, 4, false, sizeof(float) * 4);
		va.SetVertexAttributeFormat(2, GraphicsLibrary::VertexAttributeType::Float, 4, false, sizeof(float) * 8);
		va.SetVertexAttributeFormat(3, GraphicsLibrary::VertexAttributeType::Float, 4, false, sizeof(float) * 12);
		va.SetVertexAttributeFormat(4, GraphicsLibrary::VertexAttributeType::Float, 2, false, sizeof(float) * 16);
		va.SetVertexAttributeFormat(5, GraphicsLibrary::VertexAttributeType::Float, 2, false, sizeof(float) * 18);
		va.SetVertexAttributeFormat(6, GraphicsLibrary::VertexAttributeType::Float, 2, false, sizeof(float) * 20);
		va.SetVertexAttributeFormat(7, GraphicsLibrary::VertexAttributeType::Float, 2, false, sizeof(float) * 22);
		va.SetVertexAttributeFormat(7, GraphicsLibrary::VertexAttributeType::Float, 2, false, sizeof(float) * 24);
		va.SetVertexAttributeFormat(8, GraphicsLibrary::VertexAttributeType::Float, 4, false, sizeof(float) * 26);
		va.SetVertexAttributeFormat(9, GraphicsLibrary::VertexAttributeType::Float, 1, false, sizeof(float) * 30);
		va.SetVertexAttributeBuffer(0, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(1, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(2, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(3, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(4, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(5, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(6, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(7, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(8, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(9, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(10, &renderer.GetBuffer(), sizeof(Vertex), 0);

		renderer.Allocate(4, sizeof(Vertex));
	}
	ImageRenderer::~ImageRenderer()
	{
	}
	Result ImageRenderer::SetProjectionMatrix(Mat4f mat)
	{
		proj = mat;
		return Result();
	}
	Result ImageRenderer::Render(Transform2D transform, const ImageGraphicsData& data) const
	{
		return Render(transform.GetMatrix(data.size), data);
	}

	Result ImageRenderer::Render(Mat4f matrix, const ImageGraphicsData& data) const
	{
		CHECK_RESULT(CheckProgram());

		CHECK_RESULT(Graphics::Renderer::SetActiveTextureSlot(0));
		CHECK_RESULT(Graphics::Renderer::SelectTexture(data.texture));

		CHECK_RESULT(Graphics::Renderer::SelectVertexArray(&va));
		CHECK_RESULT(Graphics::Renderer::SelectProgram(&program));
		CHECK_RESULT(program.SetUniform(0, proj));
		CHECK_RESULT(program.SetUniform(1, 0));

		Vertex vertex;
		vertex.matrix = matrix;
		memcpy(vertex.uvs, data.uvs, sizeof(vertex.uvs));
		
		vertex.size = data.size;
		vertex.mask = data.mask;
		vertex.blend = data.blendFactor;		

		CHECK_RESULT(renderer.Render(ArrayView(&vertex, 1), [&](uint vertexIndex, uint vertexCount) {
			Graphics::Renderer::RenderPrimitiveArray(Graphics::Renderer::PrimitiveType::Points, vertexIndex, vertexCount);
			}));

		return Result();
	}	
	Result ImageRenderer::SetShaderProgram(GraphicsLibrary::ShaderProgram&& program)
	{
		this->program = std::move(program);

		return Result();
	}
	Result ImageRenderer::LoadDefaultShaderProgram() const
	{
		GraphicsLibrary::VertexShader vert;
		GraphicsLibrary::GeometryShader geom;
		GraphicsLibrary::FragmentShader frag;

		CHECK_RESULT(vert.Load("assets/default/shaders/image/image.vert"));
		CHECK_RESULT(geom.Load("assets/default/shaders/image/image.geom"));
		CHECK_RESULT(frag.Load("assets/default/shaders/image/image.frag"));

		if (program.GetState() != GraphicsLibrary::ShaderProgramState::Invalid)
			program = std::move(GraphicsLibrary::ShaderProgram());

		CHECK_RESULT(program.LinkShaders({ &vert, &geom, &frag }));

		return Result();
	}
	Result ImageRenderer::CheckProgram() const
	{
		switch (program.GetState())
		{
		case GraphicsLibrary::ShaderProgramState::Invalid:
			CHECK_RESULT(LoadDefaultShaderProgram());
		case GraphicsLibrary::ShaderProgramState::SuccesfullyLinked:
			return Result();
		case GraphicsLibrary::ShaderProgramState::UnsuccesfullyLinked:
			return BLAZE_ERROR_RESULT("Blaze Engine", "Trying to draw a 2D texture but the shader program wasn't succesfully linked");
		default:
			return BLAZE_ERROR_RESULT("Blaze Engine", "Invalid ShaderProgramState value");
		}
	}
}

