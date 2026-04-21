#include "pch.h"
#include "BlazeEngine/Runtime/App/AppLayer.h"
#include "BlazeEngine/Runtime/App/AppRuntimeThread.h"

namespace Blaze
{
    extern thread_local AppRuntimeThread* threadAppRuntimeThreadObject;
     
    AppLayer::AppLayer() 
        : thread(*threadAppRuntimeThreadObject), resourceManager(threadAppRuntimeThreadObject->resourceManager)
    {
    }
    AppLayer::~AppLayer() 
    {
    }
}