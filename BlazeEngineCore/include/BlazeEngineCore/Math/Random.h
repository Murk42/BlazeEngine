#pragma once
#include "BlazeEngineCore/DataStructures/Color.h"


namespace Blaze::Random
{
    BLAZE_CORE_API void SetSeed(uint seed);
    BLAZE_CORE_API float Float();
    BLAZE_CORE_API float Float(float min, float max);   
    BLAZE_CORE_API Blaze::ColorRGB ColorRGB();
    BLAZE_CORE_API Blaze::ColorRGBf ColorRGBf();
    BLAZE_CORE_API Blaze::ColorRGBA ColorRGBA();
    BLAZE_CORE_API Blaze::ColorRGBAf ColorRGBAf();

    BLAZE_CORE_API float Simplex(float v);
    BLAZE_CORE_API float Simplex(Vec2f v);
    BLAZE_CORE_API float Simplex(Vec3f v);
    BLAZE_CORE_API double Simplex(double v);
    BLAZE_CORE_API double Simplex(Vec2d v);
    BLAZE_CORE_API double Simplex(Vec3d v);
}