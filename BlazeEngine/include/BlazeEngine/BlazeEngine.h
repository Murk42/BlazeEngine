#pragma once
#include <mutex>

#include "BlazeEngine/BlazeEngineDefines.h"

#include "BlazeEngine/Console/Console.h"
#include "BlazeEngine/Console/ConsoleOutputStream.h"

#include "BlazeEngine/Window/WindowBase.h"
#include "BlazeEngine/Window/WindowSDL.h"
#include "BlazeEngine/Window/Window.h"

#include "BlazeEngine/Input/Input.h"

#include "BlazeEngine/Event/EventDispatcher.h"
#include "BlazeEngine/Event/EventHandler.h"
#include "BlazeEngine/Event/LambdaEventHandler.h"
#include "BlazeEngine/Event/QueueEventDispatcher.h"

#include "BlazeEngine/Resources/Font/Font.h"
#include "BlazeEngine/Resources/Bitmap/Bitmap.h"
#include "BlazeEngine/Resources/Font/TextLayouter.h"

#include "BlazeEngine/Application/ECS/Component.h"
#include "BlazeEngine/Application/ECS/ComponentTypeRegistry.h"
#include "BlazeEngine/Application/ECS/Entity.h"
#include "BlazeEngine/Application/ECS/EntityReference.h"
#include "BlazeEngine/Application/ECS/EntityView.h"
#include "BlazeEngine/Application/ECS/Scene.h"
#include "BlazeEngine/Application/ECS/System.h"

#include "BlazeEngine/Application/ResourceSystem/Resource.h"