#pragma once

#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"

#include "BlazeEngineGraphics/Core/Dynamic/GraphicsContext.h"

#ifdef GRAPHICS_OPENGL
#include "BlazeEngineGraphics/Core/OpenGL/Framebuffer_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/GraphicsContext_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/RenderWindowFramebuffer_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/RenderWindow_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/Semaphore_OpenGL.h"

#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLContext.h"

#include "BlazeEngineGraphics/Renderers/OpenGL/Line2DRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Renderers/OpenGL/PanelRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Renderers/OpenGL/TextRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Renderers/OpenGL/TexturedRectRenderer_OpenGL.h"
#endif

#ifdef GRAPHICS_VULKAN
#include "BlazeEngineGraphics/Vulkan/Core/Framebuffer_Vulkan.h"
#include "BlazeEngineGraphics/Vulkan/Core/GraphicsContext_Vulkan.h"
#include "BlazeEngineGraphics/Vulkan/Core/RenderWindowFramebuffer_Vulkan.h"
#include "BlazeEngineGraphics/Vulkan/Core/RenderWindow_Vulkan.h"
#include "BlazeEngineGraphics/Vulkan/Core/Semaphore_Vulkan.h"

#include "BlazeEngineGraphics/Renderers/Vulkan/Line2DRenderer_Vulkan.h"
#endif

#include "BlazeEngineGraphics/UI/Core/UINode.h"
#include "BlazeEngineGraphics/UI/Core/UIScreen.h"
#include "BlazeEngineGraphics/UI/Input/UIInputManager.h"
#include "BlazeEngineGraphics/UI/Input/UIInputNode.h"
#include "BlazeEngineGraphics/UI/Input/UIKeyboardEventHandler.h"
#include "BlazeEngineGraphics/UI/Input/UIMouseEventHandler.h"
#include "BlazeEngineGraphics/UI/Input/UISelectEventHandler.h"
#ifdef GRAPHICS_OPENGL
#include "BlazeEngineGraphics/UI/Graphics/RenderPipelines/OpenGL/UIRenderPipeline_OpenGL.h"

#include "BlazeEngineGraphics/UI/Graphics/RenderStreams/OpenGL/ImageRenderStream_OpenGL.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderStreams/OpenGL/PanelRenderStream_OpenGL.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderStreams/OpenGL/TextInputRenderStream_OpenGL.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderStreams/OpenGL/TextRenderStream_OpenGL.h"
#endif

#include "BlazeEngineGraphics/UI/Elements/UIImage.h"
#include "BlazeEngineGraphics/UI/Elements/UIInputBox.h"
#include "BlazeEngineGraphics/UI/Elements/UIPanel.h"
#include "BlazeEngineGraphics/UI/Elements/UIText.h"
#include "BlazeEngineGraphics/UI/Elements/UITextButton.h"