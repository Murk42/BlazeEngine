#pragma once

#ifndef BLAZE_GRAPHICS_OPENGL
#define BLAZE_GRAPHICS_OPENGL
#endif

namespace Blaze
{
#ifdef BLAZE_GRAPHICS_OPENGL	
	namespace OpenGL
#endif
	{
		class BLAZE_API VertexArray;
		class BLAZE_API GraphicsBuffer;

		class BLAZE_API ShaderProgram;
		class BLAZE_API Shader;
		class BLAZE_API VertexShader;
		class BLAZE_API FragmentShader;
		class BLAZE_API GeometryShader;
		class BLAZE_API TessellationControlShader;
		class BLAZE_API TessellationEvaluationShader;
		class BLAZE_API ComputeShader;

		class BLAZE_API Framebuffer;
		class BLAZE_API Renderbuffer;

		class BLAZE_API Texture1D;
		class BLAZE_API Texture2D;
		class BLAZE_API Texture3D;
		class BLAZE_API Texture2DArray;
		class BLAZE_API TextureCubemap;
		class BLAZE_API TextureBuffer;

		class BLAZE_API Fence;
	}

	namespace Graphics
	{
		namespace Core
		{
#ifdef BLAZE_GRAPHICS_OPENGL
			using namespace Blaze::OpenGL;
#endif
		}
	}
}