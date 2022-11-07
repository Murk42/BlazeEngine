#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/Color.h"

namespace Blaze::Random
{
    BLAZE_API void SetSeed(int64 seed);
    BLAZE_API float Float();
    BLAZE_API float Float(float min, float max);
    BLAZE_API Blaze::ColorRGB ColorRGB();
    BLAZE_API Blaze::ColorRGBf ColorRGBf();
    BLAZE_API Blaze::ColorRGBA ColorRGBA();
    BLAZE_API Blaze::ColorRGBAf ColorRGBAf();

    BLAZE_API float Simplex(float v);
    BLAZE_API float Simplex(Vec2f v);
    BLAZE_API float Simplex(Vec3f v);
    BLAZE_API double Simplex(double v);
    BLAZE_API double Simplex(Vec2d v);
    BLAZE_API double Simplex(Vec3d v);
}