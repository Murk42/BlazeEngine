#define BLAZE_ENGINE

#include "BlazeEngine/Core/Scene.h"
#include "BlazeEngine/Core/Logger.h"
#include "BlazeEngine/Core/Application.h"
#include "BlazeEngine/Core/Window.h"

#include "BlazeEngine/Console/Console.h"

#include "BlazeEngine/Graphics/Renderer.h"
#include "BlazeEngine/Resources/Bitmap/Bitmap.h"
#include "BlazeEngine/Graphics/GraphicsLibrary.h"
#include "BlazeEngine/Resources/Font/Font.h"
#include "BlazeEngine/Graphics/Material/Material.h"
#include "BlazeEngine/Graphics/Mesh/Mesh.h"

#include "BlazeEngine/Input/Key.h"
#include "BlazeEngine/Input/Event.h"
#include "BlazeEngine/Input/Input.h"

#include "BlazeEngine/Math/Math.h"
#include "BlazeEngine/Math/Random.h"

#include "BlazeEngine/Utilities/File.h"
#include "BlazeEngine/Utilities/FileSystem.h"
#include "BlazeEngine/Utilities/Threading.h"
#include "BlazeEngine/Utilities/ThreadWorker.h"
#include "BlazeEngine/Utilities/Stopwatch.h"
#include "BlazeEngine/Utilities/Time.h"

#include "BlazeEngine/DataStructures/Array.h"
#include "BlazeEngine/DataStructures/Color.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/DataStructures/StringView.h"
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/DataStructures/AxisAngle.h"
#include "BlazeEngine/DataStructures/Quaternion.h"
#include "BlazeEngine/DataStructures/Matrix.h"
#include "BlazeEngine/DataStructures/Guard.h"
#include "BlazeEngine/DataStructures/ByteStream.h"
#include "BlazeEngine/DataStructures/StringStream.h"
#include "BlazeEngine/DataStructures/Rect.h"
#include "BlazeEngine/DataStructures/Transform2D.h"
#include "BlazeEngine/DataStructures/Transform3D.h"
#include "BlazeEngine/DataStructures/Tuple.h"
#include "BlazeEngine/DataStructures/Template.h"
#include "BlazeEngine/DataStructures/TemplateGroup.h"
#include "BlazeEngine/DataStructures/Buffer.h"

/*
#include "BlazeEngine/Graphics/UI/Layer.h"
#include "BlazeEngine/Graphics/UI/UI.h"

#include "BlazeEngine/Graphics/UI/Elements/ElementTransform.h"
#include "BlazeEngine/Graphics/UI/Elements/Element.h"
#include "BlazeEngine/Graphics/UI/Elements/Panel.h"
#include "BlazeEngine/Graphics/UI/Elements/Text.h"

#include "BlazeEngine/Graphics/UI/States/ButtonState.h"
#include "BlazeEngine/Graphics/UI/States/TextFieldState.h"
#include "BlazeEngine/Graphics/UI/States/State.h"
*/

#include "BlazeEngine/Debug/Graphics.h"