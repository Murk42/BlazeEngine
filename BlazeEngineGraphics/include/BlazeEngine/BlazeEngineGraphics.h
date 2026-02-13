#pragma once

#include "BlazeEngine/Graphics/Core/GraphicsContextBase.h"

#ifdef GRAPHICS_OPENGL
#include "BlazeEngine/Graphics/Core/OpenGL/Framebuffer_OpenGL.h"
#include "BlazeEngine/Graphics/Core/OpenGL/GraphicsContext_OpenGL.h"
#include "BlazeEngine/Graphics/Core/OpenGL/RenderWindowFramebuffer_OpenGL.h"
#include "BlazeEngine/Graphics/Core/OpenGL/RenderWindow_OpenGL.h"
#include "BlazeEngine/Graphics/Core/OpenGL/Semaphore_OpenGL.h"

#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLFence.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLGraphicsBuffer.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLProgram.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLRenderbuffer.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLShader.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLVertexArray.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/Textures/OpenGLTexture1D.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/Textures/OpenGLTexture2D.h"
//#include "BlazeEngine/Graphics/Renderers/OpenGL/Line2DRenderer_OpenGL.h"
//#include "BlazeEngine/Graphics/Renderers/OpenGL/Line3DRenderer_OpenGL.h"
#include "BlazeEngine/Graphics/Renderers/OpenGL/PanelRenderer_OpenGL.h"
#include "BlazeEngine/Graphics/Renderers/OpenGL/TexturedRectRenderer_OpenGL.h"
#include "BlazeEngine/Graphics/Renderers/OpenGL/BasicIndexedMeshRenderer_OpenGL.h"
#include "BlazeEngine/Graphics/Renderers/OpenGL/Line2DRenderer_OpenGL.h"
#endif

#ifdef GRAPHICS_VULKAN
#include "BlazeEngine/Graphics/Vulkan/Core/Framebuffer_Vulkan.h"
#include "BlazeEngine/Graphics/Vulkan/Core/GraphicsContext_Vulkan.h"
#include "BlazeEngine/Graphics/Vulkan/Core/RenderWindowFramebuffer_Vulkan.h"
#include "BlazeEngine/Graphics/Vulkan/Core/RenderWindow_Vulkan.h"
#include "BlazeEngine/Graphics/Vulkan/Core/Semaphore_Vulkan.h"

#include "BlazeEngine/Graphics/Renderers/Vulkan/Line2DRenderer_Vulkan.h"
#endif