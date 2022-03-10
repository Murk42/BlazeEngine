#define BLAZE_ENGINE

#include "BlazeEngine/Logger/Log.h"
#include "BlazeEngine/Logger/Logger.h"
#include "BlazeEngine/Logger/LogListener.h"
#include "BlazeEngine/Core/Window.h" 
#include "BlazeEngine/Core/MemoryManager.h" 

#include "BlazeEngine/Application/Application.h"

#include "BlazeEngine/Console/Console.h"

#include "BlazeEngine/Graphics/Graphics.h"
#include "BlazeEngine/Graphics/Renderers/Line2DRenderer.h"
#include "BlazeEngine/Graphics/Renderers/Line3DRenderer.h"
#include "BlazeEngine/Graphics/Renderers/Point2DRenderer.h"
#include "BlazeEngine/Graphics/Renderers/TextRenderer.h"
#include "BlazeEngine/Graphics/Renderer.h"
#include "BlazeEngine/Resources/Bitmap/Bitmap.h"
#include "BlazeEngine/Graphics/GraphicsLibrary.h"
#include "BlazeEngine/Resources/Font/Font.h"

#include "BlazeEngine/Input/Key.h"
#include "BlazeEngine/Input/Input.h"

#include "BlazeEngine/Event/Events.h"
#include "BlazeEngine/Event/EventDispatcher.h"
#include "BlazeEngine/Event/EventHandler.h"
#include "BlazeEngine/Event/EventManager.h"

#include "BlazeEngine/Math/Math.h"
#include "BlazeEngine/Math/Random.h"

#include "BlazeEngine/File/Path.h"
#include "BlazeEngine/File/File.h"
#include "BlazeEngine/File/FileSystem.h"

#include "BlazeEngine/Utilities/Stopwatch.h"
#include "BlazeEngine/Utilities/Time.h"
#include "BlazeEngine/Utilities/Threading.h"
#include "BlazeEngine/Utilities/Thread.h"

#include "BlazeEngine/DataStructures/Color.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/DataStructures/StringUTF8.h"
#include "BlazeEngine/DataStructures/StringView.h"
#include "BlazeEngine/DataStructures/StringViewUTF8.h"
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/DataStructures/Quaternion.h"
#include "BlazeEngine/DataStructures/Matrix.h"
#include "BlazeEngine/DataStructures/Rect.h"
#include "BlazeEngine/DataStructures/Transform2D.h"
#include "BlazeEngine/DataStructures/Transform3D.h"
#include "BlazeEngine/DataStructures/Tuple.h"
#include "BlazeEngine/DataStructures/Template.h"
#include "BlazeEngine/DataStructures/TemplateGroup.h"
#include "BlazeEngine/DataStructures/Buffer.h"
#include "BlazeEngine/DataStructures/StringHash.h"