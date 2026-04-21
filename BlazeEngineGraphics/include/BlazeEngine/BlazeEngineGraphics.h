#pragma once

#include "BlazeEngine/Graphics/Core/FramebufferBase.h"
#include "BlazeEngine/Graphics/Core/GraphicsContext.h"
#include "BlazeEngine/Graphics/Core/RenderContext.h"
#include "BlazeEngine/Graphics/Core/RendererBase.h"
#include "BlazeEngine/Graphics/Core/RendererRegistry.h"

#include "BlazeEngine/Graphics/Renderers/BasicIndexedMeshRenderer.h"
#include "BlazeEngine/Graphics/Renderers/Line2DRenderer.h"
#include "BlazeEngine/Graphics/Renderers/PanelRenderer.h"
#include "BlazeEngine/Graphics/Renderers/Quad2DRenderer.h"
#include "BlazeEngine/Graphics/Renderers/TexturedRectRenderer.h"

#include "BlazeEngine/Graphics/Common/BasicIndexedMesh.h"

#include "BlazeEngine/Graphics/App/GraphicsAppLayer.h"
#include "BlazeEngine/Graphics/App/GraphicsAppRuntimeThread.h"

#if not (defined(BLAZE_GRAPHICS_OPENGL))
#define BLAZE_GRAPHICS_OPENGL
#endif

#ifdef BLAZE_GRAPHICS_OPENGL
#include "BlazeEngine/Graphics/GraphicsAPI_OpenGL.h"

namespace Blaze::Graphics
{
	using namespace OpenGL;
}
#endif


#ifdef GRAPHICS_VULKAN
#include "BlazeEngine/Graphics/Vulkan/Core/Framebuffer_Vulkan.h"
#include "BlazeEngine/Graphics/Vulkan/Core/GraphicsContext_Vulkan.h"
#include "BlazeEngine/Graphics/Vulkan/Core/RenderWindowFramebuffer_Vulkan.h"
#include "BlazeEngine/Graphics/Vulkan/Core/RenderWindow_Vulkan.h"
#include "BlazeEngine/Graphics/Vulkan/Core/Semaphore_Vulkan.h"

#include "BlazeEngine/Graphics/Renderers/Vulkan/Line2DRenderer_Vulkan.h"
#endif