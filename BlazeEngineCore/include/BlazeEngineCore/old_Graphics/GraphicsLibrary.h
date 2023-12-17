#pragma once

#include "GraphicsLibraryType.h"

#ifdef BLAZE_GRAPHICS_OPENGL
#include "BlazeEngineCore/Graphics/OpenGL/OpenGLGraphicsBuffer.h"
#include "BlazeEngineCore/Graphics/OpenGL/OpenGLProgram.h"
#include "BlazeEngineCore/Graphics/OpenGL/OpenGLShader.h"
#include "BlazeEngineCore/Graphics/OpenGL/OpenGLVertexArray.h"
#include "BlazeEngineCore/Graphics/OpenGL/OpenGLFramebuffer.h"
#include "BlazeEngineCore/Graphics/OpenGL/OpenGLRenderbuffer.h"
#include "BlazeEngineCore/Graphics/OpenGL/Textures/OpenGLTexture1D.h"
#include "BlazeEngineCore/Graphics/OpenGL/Textures/OpenGLTexture2D.h"
#include "BlazeEngineCore/Graphics/OpenGL/Textures/OpenGLTexture3D.h"
#include "BlazeEngineCore/Graphics/OpenGL/Textures/OpenGLTexture2DArray.h"
#include "BlazeEngineCore/Graphics/OpenGL/Textures/OpenGLTextureCubemap.h"
#include "BlazeEngineCore/Graphics/OpenGL/Textures/OpenGLTextureBuffer.h"
#include "BlazeEngineCore/Graphics/OpenGL/Textures/OpenGLTextureEnums.h"
#include "BlazeEngineCore/Graphics/OpenGL/OpenGLFence.h"
#endif

namespace Blaze
{
	namespace GraphicsLibrary
	{
#ifdef BLAZE_GRAPHICS_OPENGL
		using namespace ::Blaze::OpenGL;
#endif
	}	
}