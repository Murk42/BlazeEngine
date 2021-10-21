#pragma once
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

enum BlockID
{
	Air,
	Full,	
};

struct Block
{
	BlockID id;
	ColorRGB color;
};

inline bool IsFullBlock(Block& block) 
{
	return block.id != BlockID::Air;
}