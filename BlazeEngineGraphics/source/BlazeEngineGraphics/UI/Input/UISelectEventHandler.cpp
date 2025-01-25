#include "pch.h"
#include "BlazeEngineGraphics/UI/Input/UISelectEventHandler.h"

namespace Blaze::UI
{
    UISelectEventHandler::UISelectEventHandler()
        : selectable(true)
    {
    }
    void UISelectEventHandler::SetSelectableFlag(bool selectable)
    {
        this->selectable = selectable;
    }
}