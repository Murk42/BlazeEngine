#include "BlazeEngine/Graphics/UI/States/State.h"
#include "BlazeEngine/Graphics/UI/States/StateManager.h"

namespace Blaze::UI
{
    void State::Select()
    {
        manager->Select(*this);
    }
}