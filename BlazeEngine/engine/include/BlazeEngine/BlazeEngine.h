#pragma once
#define BLAZE_ENGINE

#include "BlazeEnginePrecompiledHeaders.h"

#include "BlazeEngine/Application/ResourceSystem/Resource.h"
#include "BlazeEngine/Application/ResourceSystem/ResourceManager.h"
#include "BlazeEngine/Application/ResourceSystem/ResourceStorage.h"
#include "BlazeEngine/Application/ResourceSystem/ResourceTypeRegistry.h"

#include "BlazeEngine/Application/ECS/Component.h"
#include "BlazeEngine/Application/ECS/ComponentTypeRegistry.h"
#include "BlazeEngine/Application/ECS/Entity.h"
#include "BlazeEngine/Application/ECS/EntityReference.h"
#include "BlazeEngine/Application/ECS/Scene.h"
#include "BlazeEngine/Application/ECS/System.h"

#include "BlazeEngine/Application/UI/UIScene.h"
#include "BlazeEngine/Application/UI/UIComponent.h"
#include "BlazeEngine/Application/UI/UISystem.h"
#include "BlazeEngine/Application/UI/Components/Bases/UIPosOverrideComponent.h"
#include "BlazeEngine/Application/UI/Components/Bases/UISizeOverrideComponent.h"
#include "BlazeEngine/Application/UI/Components/UIElementComponent.h"
#include "BlazeEngine/Application/UI/Components/UITransformComponent.h"
#include "BlazeEngine/Application/UI/Components/LabelComponent.h"
#include "BlazeEngine/Application/UI/Components/FrameComponent.h"
#include "BlazeEngine/Application/UI/Components/ImageComponent.h"
#include "BlazeEngine/Application/UI/Components/MouseTriggerComponent.h"

#include "BlazeEngine/Console/Console.h"

#include "BlazeEngine/Core/Window.h" 

#include "BlazeEngine/DataStructures/Color.h"
#include "BlazeEngine/DataStructures/Rect.h"
#include "BlazeEngine/DataStructures/Tuple.h"
#include "BlazeEngine/DataStructures/Template.h"
#include "BlazeEngine/DataStructures/TemplateGroup.h"
#include "BlazeEngine/DataStructures/Buffer.h"
#include "BlazeEngine/DataStructures/HybridArray.h"
#include "BlazeEngine/DataStructures/Handle.h"
#include "BlazeEngine/DataStructures/HandleImpl.h"

#include "BlazeEngine/Event/EventDispatcher.h"
#include "BlazeEngine/Event/EventHandler.h"
#include "BlazeEngine/Event/LambdaEventHandler.h"

#include "BlazeEngine/File/Path.h"
#include "BlazeEngine/File/File.h"
#include "BlazeEngine/File/FileSystem.h"
#include "BlazeEngine/File/Stream/BufferStream.h"
#include "BlazeEngine/File/Stream/FileStream.h"
#include "BlazeEngine/File/Stream/Stream.h"
#include "BlazeEngine/File/Stream/SubStream.h"

#include "BlazeEngine/Input/Key.h"
#include "BlazeEngine/Input/Input.h"

#include "BlazeEngine/Math/Quaternion.h"
#include "BlazeEngine/Math/Matrix.h"
#include "BlazeEngine/Math/Math.h"
#include "BlazeEngine/Math/Random.h"
#include "BlazeEngine/Math/Shapes.h"

#include "BlazeEngine/Resources/Bitmap/Bitmap.h"
#include "BlazeEngine/Resources/Font/Font.h"

#include "BlazeEngine/Utilities/Stopwatch.h"
#include "BlazeEngine/Utilities/Timing.h"
#include "BlazeEngine/Utilities/Threading.h"

#include "BlazeEngine/Threading/Thread.h"

#include "BlazeEngine/Memory/MallocAllocator.h"
#include "BlazeEngine/Memory/MemoryListener.h"

#include "BlazeEngine/Core/Setup.h"