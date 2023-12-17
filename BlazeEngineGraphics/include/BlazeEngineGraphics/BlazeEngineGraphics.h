#pragma once

#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"

#include "BlazeEngineGraphics/Dynamic/Core/GraphicsContext.h"

//#ifdef GRAPHICS_OPENGL
#include "BlazeEngineGraphics/OpenGL/Core/Framebuffer_OpenGL.h"
#include "BlazeEngineGraphics/OpenGL/Core/GraphicsContext_OpenGL.h"
#include "BlazeEngineGraphics/OpenGL/Core/RenderWindowFramebuffer_OpenGL.h"
#include "BlazeEngineGraphics/OpenGL/Core/RenderWindow_OpenGL.h"
#include "BlazeEngineGraphics/OpenGL/Core/Semaphore_OpenGL.h"

#include "BlazeEngineGraphics/OpenGLWrapper/OpenGLContext.h"

#include "BlazeEngineGraphics/Renderers/OpenGL/Line2DRenderer_OpenGL.h"
//#endif

//#ifdef GRAPHICS_VULKAN
#include "BlazeEngineGraphics/Vulkan/Core/Framebuffer_Vulkan.h"
#include "BlazeEngineGraphics/Vulkan/Core/GraphicsContext_Vulkan.h"
#include "BlazeEngineGraphics/Vulkan/Core/RenderWindowFramebuffer_Vulkan.h"
#include "BlazeEngineGraphics/Vulkan/Core/RenderWindow_Vulkan.h"
#include "BlazeEngineGraphics/Vulkan/Core/Semaphore_Vulkan.h"

#include "BlazeEngineGraphics/Renderers/Vulkan/Line2DRenderer_Vulkan.h"
//#endif