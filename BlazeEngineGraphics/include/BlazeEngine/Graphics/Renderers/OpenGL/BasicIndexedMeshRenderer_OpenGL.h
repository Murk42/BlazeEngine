#pragma once
#include "BlazeEngine/Graphics/Core/OpenGL/GraphicsContext_OpenGL.h"
#include "BlazeEngine/Graphics/Renderers/BasicIndexedMeshRenderer.h"
#include "BlazeEngine/Graphics/Renderers/OpenGL/BufferedRendererBase_OpenGL.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLVertexArray.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLFence.h"

namespace Blaze::Graphics::OpenGL
{
	class BLAZE_API BasicIndexedMeshRenderer_OpenGL : public BasicIndexedMeshRenderer, public BufferedRendererBase_OpenGL
	{
	public:
		BasicIndexedMeshRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext, bool loadDefaultShaders = true);
		~BasicIndexedMeshRenderer_OpenGL();

		void StartRender(const RenderContext_OpenGL& context) override;
		void EndRender() override;

		void SetProjectionMatrix(const Mat4f& matrix) override;
		void SetViewMatrix(const Mat4f& matrix) override;
		void SetShadingOptions(Vec3f lightDirection, ColorRGBf ambientColor = 0x000000_rgb) override;

		void Render(ArrayView<Vec3f> vertices, ArrayView<uint32> indices, Mat4f modelMatrix, ColorRGBAf color) override;
	private:
		VertexArray va;
		ImmutableMappedGraphicsBuffer meshDataBuffer;
		void* meshDataMap;

		uintMem vertexCount;
		uintMem indexCount;
		uintMem meshCount;

		Fence fence;

		bool rendering;

		void WaitFence();

		void CopySubMesh(ArrayView<Vec3f> vertices, ArrayView<uint32> indices);

		void Flush();
	};
}