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
#include "BlazeEngine/Graphics/OpenGL/OpenGLRenderbuffer.h"
#include "BlazeEngine/Graphics/OpenGL/Textures/OpenGLTexture1D.h"
#include "BlazeEngine/Graphics/OpenGL/Textures/OpenGLTexture2D.h"
#include "BlazeEngine/Graphics/OpenGL/Textures/OpenGLTexture3D.h"
#include "BlazeEngine/Graphics/OpenGL/Textures/OpenGLTexture2DArray.h"
#include "BlazeEngine/Graphics/OpenGL/Textures/OpenGLTextureCubemap.h"
#include "BlazeEngine/Graphics/OpenGL/Textures/OpenGLTextureBuffer.h"
#include "BlazeEngine/Graphics/OpenGL/Textures/OpenGLTextureEnums.h"
#include "BlazeEngine/Graphics/OpenGL/OpenGLFence.h"
#endif

namespace Blaze
{
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