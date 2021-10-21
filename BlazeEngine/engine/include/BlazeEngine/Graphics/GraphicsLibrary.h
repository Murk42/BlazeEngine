#pragma once

#ifndef BLAZE_GRAPHICS_OPENGL
#define BLAZE_GRAPHICS_OPENGL
#endif

#ifdef BLAZE_GRAPHICS_OPENGL
#include "BlazeEngine/Graphics/OpenGL/OpenGLGraphicsBuffer.h"
#include "BlazeEngine/Graphics/OpenGL/OpenGLProgram.h"
#include "BlazeEngine/Graphics/OpenGL/OpenGLShader.h"
#include "BlazeEngine/Graphics/OpenGL/OpenGLVertexArray.h"
#include "BlazeEngine/Graphics/OpenGL/OpenGLFramebuffer.h"
#include "BlazeEngine/Graphics/OpenGL/Textures/OpenGLTexture2D.h"
#include "BlazeEngine/Graphics/OpenGL/Textures/OpenGLTexture2DArray.h"
#include "BlazeEngine/Graphics/OpenGL/Textures/OpenGLTextureCubemap.h"
#include "BlazeEngine/Graphics/OpenGL/Textures/OpenGLTextureEnums.h"
#endif

namespace Blaze
{
	namespace Graphics
	{
#ifdef BLAZE_GRAPHICS_OPENGL
		namespace Core = Blaze::OpenGL;
#endif
	}
}