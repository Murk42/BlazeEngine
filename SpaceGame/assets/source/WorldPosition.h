#pragma once
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

struct Fixed
{
	uint32 value;
};

constexpr Fixed operator ""t(long double value)
{
	return { (uint32)(value * (1ll << 16)) };
}

struct WorldPosition
{
	Vec3<Fixed> localPos;
	Vec3<uint64> regionPos;
};

struct SpaceBody
{
	WorldPosition worldPosition;
};

struct Region
{
	static constexpr uint32 regionSize = 1ll << 16;
	Vec3<uint64> regionPos;

	std::vector<SpaceBody> spaceBodies;
};