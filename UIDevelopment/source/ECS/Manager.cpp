#include "Manager.h"

namespace Blaze::ECS
{
    Result Manager::SetRegistry(const ComponentTypeRegistry& registry)
    {
        if (this->registry.GetComponentTypeCount() != 0)
            return BLAZE_ERROR_RESULT("BlazeEngine", "Cant change the element registry");

        this->registry = registry;

        return Result();
    }
}