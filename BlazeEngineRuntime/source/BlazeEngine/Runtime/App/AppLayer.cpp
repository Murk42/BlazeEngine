#include "pch.h"
#include "BlazeEngine/Runtime/App/AppLayer.h"

namespace Blaze
{
    AppLayer::AppLayer()
        : scheduledForDestruction(false)
    {
    }
    AppLayer::~AppLayer() 
    {
        destructionEventDispatcher.Call({ *this });
    }
    void AppLayer::ScheduleForDestruction()
    {
        scheduledForDestruction = true;
    }

    bool AppLayer::IsScheduledForDesturction() const
    {
        return scheduledForDestruction;
    }
}