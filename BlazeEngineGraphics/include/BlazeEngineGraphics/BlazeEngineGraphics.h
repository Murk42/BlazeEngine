#pragma once
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"

#include "BlazeEngineGraphics/Core/Dynamic/GraphicsContext.h"

#ifdef GRAPHICS_OPENGL
#include "BlazeEngineGraphics/Core/OpenGL/Framebuffer_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/GraphicsContext_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/RenderWindowFramebuffer_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/RenderWindow_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/Semaphore_OpenGL.h"

#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLFence.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLGraphicsBuffer.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLProgram.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLRenderbuffer.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLShader.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLVertexArray.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/Textures/OpenGLTexture1D.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/Textures/OpenGLTexture2D.h"

#include "BlazeEngineGraphics/Renderers/OpenGL/Line2DRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Renderers/OpenGL/Line3DRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Renderers/OpenGL/PanelRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Renderers/OpenGL/ColoredCharacterRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Renderers/OpenGL/TexturedRectRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Renderers/OpenGL/BasicMeshRenderer_OpenGL.h"
#endif

#ifdef GRAPHICS_VULKAN
#include "BlazeEngineGraphics/Vulkan/Core/Framebuffer_Vulkan.h"
#include "BlazeEngineGraphics/Vulkan/Core/GraphicsContext_Vulkan.h"
#include "BlazeEngineGraphics/Vulkan/Core/RenderWindowFramebuffer_Vulkan.h"
#include "BlazeEngineGraphics/Vulkan/Core/RenderWindow_Vulkan.h"
#include "BlazeEngineGraphics/Vulkan/Core/Semaphore_Vulkan.h"

#include "BlazeEngineGraphics/Renderers/Vulkan/Line2DRenderer_Vulkan.h"
#endif

//Text
#include "BlazeEngineGraphics/Text/Core/FontAtlas.h"
#include "BlazeEngineGraphics/Text/Core/FontGlyphRenderer.h"

//UI
#include "BlazeEngineGraphics/UI/Core/NodeDataMap.h"
#include "BlazeEngineGraphics/UI/Core/NodeTransform.h"
#include "BlazeEngineGraphics/UI/Core/NodeFinalTransform.h"
#include "BlazeEngineGraphics/UI/Core/NodeTreeIterator.h"
#include "BlazeEngineGraphics/UI/Core/NodeTreeView.h"
#include "BlazeEngineGraphics/UI/Core/Node.h"
#include "BlazeEngineGraphics/UI/Core/Screen.h"

//---UI Graphics
#ifdef GRAPHICS_OPENGL
#include "BlazeEngineGraphics/UI/Graphics/RenderPipelines/OpenGL/UIRenderPipeline_OpenGL.h"
#endif
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/ImageRenderUnit.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/PanelRenderUnit.h"

//---UI Input
#include "BlazeEngineGraphics/UI/Input/InputManager.h"
#include "BlazeEngineGraphics/UI/Input/InputNode.h"

#include "BlazeEngineGraphics/UI/Nodes/EmptyNode.h"
#include "BlazeEngineGraphics/UI/Nodes/Panel.h"
#include "BlazeEngineGraphics/UI/Nodes/Image.h"
#include "BlazeEngineGraphics/UI/Nodes/Text.h"
//#include "BlazeEngineGraphics/UI/Nodes/TextButton.h"
//#include "BlazeEngineGraphics/UI/Nodes/EditableText.h"
//#include "BlazeEngineGraphics/UI/Nodes/SelectableText.h"