#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/DataStructures/Common.h"

namespace Blaze
{
    class BLAZE_API Random
    {
    public:
        static void SetSeed(int64 seed);
        static float Float();
        static float Float(float min, float max);

        static float Simplex(float v);
        static float Simplex(Vec2f v);
        static float Simplex(Vec3f v);
        static double Simplex(double v);
        static double Simplex(Vec2d v);
        static double Simplex(Vec3d v);
    };
}