#define BLAZE_ENGINE

//---Engine PCH---
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/DataStructures/StringHash.h"
#include "BlazeEngine/DataStructures/StringView.h"
#include "BlazeEngine/Utilities/Time.h"
#include "BlazeEngine/Logging/Log.h"
#include "BlazeEngine/Core/Result.h"
#include "BlazeEngine/Core/ResultValue.h"
#include "BlazeEngine/Logging/Logger.h"
#include "BlazeEngine/Utilities/StringParsing.h"
#include "BlazeEngine/Memory/MemoryManager.h"
//----------------

#include "BlazeEngine/Application/ResourceSystem/Resource.h"
#include "BlazeEngine/Application/ResourceSystem/ResourceManager.h"
#include "BlazeEngine/Application/ResourceSystem/ResourceStorage.h"
#include "BlazeEngine/Application/ResourceSystem/ResourceTypeRegistry.h"
#include "BlazeEngine/Application/ECS/Component.h"
#include "BlazeEngine/Application/ECS/ComponentTypeRegistry.h"
#include "BlazeEngine/Application/ECS/Entity.h"
#include "BlazeEngine/Application/ECS/Manager.h"
#include "BlazeEngine/Application/ECS/Scene.h"
#include "BlazeEngine/Application/ECS/System.h"

#include "BlazeEngine/Console/Console.h"

#include "BlazeEngine/Core/Window.h" 
#include "BlazeEngine/Core/Startup.h"

#include "BlazeEngine/DataStructures/Color.h"
#include "BlazeEngine/DataStructures/StringUTF8.h"
#include "BlazeEngine/DataStructures/StringViewUTF8.h"
#include "BlazeEngine/DataStructures/Quaternion.h"
#include "BlazeEngine/DataStructures/Matrix.h"
#include "BlazeEngine/DataStructures/Rect.h"
#include "BlazeEngine/DataStructures/Transform2D.h"
#include "BlazeEngine/DataStructures/Transform3D.h"
#include "BlazeEngine/DataStructures/Tuple.h"
#include "BlazeEngine/DataStructures/Template.h"
#include "BlazeEngine/DataStructures/TemplateGroup.h"
#include "BlazeEngine/DataStructures/Buffer.h"
#include "BlazeEngine/DataStructures/HybridArray.h"

#include "BlazeEngine/Event/Events.h"
#include "BlazeEngine/Event/EventDispatcher.h"
#include "BlazeEngine/Event/EventHandler.h"
#include "BlazeEngine/Event/EventManager.h"

#include "BlazeEngine/File/Path.h"
#include "BlazeEngine/File/File.h"
#include "BlazeEngine/File/FileSystem.h"

#include "BlazeEngine/Graphics/Graphics.h"
#include "BlazeEngine/Graphics/Renderers/Line2DRenderer.h"
#include "BlazeEngine/Graphics/Renderers/Line3DRenderer.h"
#include "BlazeEngine/Graphics/Renderers/Point2DRenderer.h"
#include "BlazeEngine/Graphics/Renderers/TextRenderer.h"
#include "BlazeEngine/Graphics/Renderers/TextRenderCache.h"
#include "BlazeEngine/Graphics/Renderers/TextVertexGenerator.h"
#include "BlazeEngine/Graphics/GraphicsCore.h"
#include "BlazeEngine/Graphics/GraphicsLibrary.h"

#include "BlazeEngine/Input/Key.h"
#include "BlazeEngine/Input/Input.h"

#include "BlazeEngine/Logging/LogListener.h"

#include "BlazeEngine/Math/Math.h"
#include "BlazeEngine/Math/Random.h"

#include "BlazeEngine/Resources/Bitmap/Bitmap.h"
#include "BlazeEngine/Resources/Font/Font.h"

#include "BlazeEngine/Utilities/Stopwatch.h"
#include "BlazeEngine/Utilities/Thread.h"
#include "BlazeEngine/Utilities/Threading.h"

#include "BlazeEngine/Memory/MallocAllocator.h"
#include "BlazeEngine/Memory/MemoryListener.h"

#define CLIENT_API extern "C" __declspec(dllexport)