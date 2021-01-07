#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Vector.h"

namespace Blaze
{
    class BLAZE_API Random
    {
    public:
        static float Float();
        static float Float(float min, float max);

        static float Simplex(float v);
        static float Simplex(Vec2f v);
        static float Simplex(Vec3f v);
    };
}