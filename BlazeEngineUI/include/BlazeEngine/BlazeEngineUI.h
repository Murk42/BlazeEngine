#pragma once

#include "BlazeEngine/UI/Text/CharacterSet.h"
#include "BlazeEngine/UI/Common/ButtonBase.h"
#include "BlazeEngine/UI/Common/System.h"

#include "BlazeEngine/UI/Core/NodeDataMap.h"
#include "BlazeEngine/UI/Core/NodeTransform.h"
#include "BlazeEngine/UI/Core/NodeFinalTransform.h"
#include "BlazeEngine/UI/Core/NodeTreeIterator.h"
#include "BlazeEngine/UI/Core/NodeTreeView.h"
#include "BlazeEngine/UI/Core/Node.h"
#include "BlazeEngine/UI/Core/Screen.h"

#include "BlazeEngine/UI/Text/Font.h"
#include "BlazeEngine/UI/Text/FontAtlas.h"
#include "BlazeEngine/UI/Text/FontFace.h"
#include "BlazeEngine/UI/Text/FontGlyphRasterizer.h"
#include "BlazeEngine/UI/Text/FontManager.h"

#include "BlazeEngine/UI/Graphics/RenderNode.h"
#include "BlazeEngine/UI/Graphics/GraphicsSubSystem.h"
#include "BlazeEngine/UI/Graphics/RenderContext.h"
#include "BlazeEngine/UI/Graphics/RenderUnit.h"
#include "BlazeEngine/UI/Graphics/Renderers/TextRendererBase.h"
#include "BlazeEngine/UI/Graphics/Renderers/AntialiasedTextRenderer.h"
#ifdef GRAPHICS_OPENGL
#include "BlazeEngine/UI/Graphics/Renderers/OpenGL/AntialiasedTextRenderer_OpenGL.h"
#endif

#include "BlazeEngine/UI/Graphics/RenderUnits/ImageRenderUnit.h"

#include "BlazeEngine/UI/Input/InputSubSystem.h"
#include "BlazeEngine/UI/Input/InputNode.h"
#include "BlazeEngine/UI/Input/PointerData.h"

#include "BlazeEngine/UI/Nodes/EmptyNode.h"
#include "BlazeEngine/UI/Nodes/Panel.h"
#include "BlazeEngine/UI/Nodes/Image.h"
#include "BlazeEngine/UI/Nodes/Label.h"
#include "BlazeEngine/UI/Nodes/PanelButton.h"
#include "BlazeEngine/UI/Nodes/Slider.h"