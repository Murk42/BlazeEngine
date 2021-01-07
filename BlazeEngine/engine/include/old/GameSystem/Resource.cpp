#include "Resource.h"
#include "BlazeEngine/GameSystem/SceneManager.h"
#include "BlazeEngine/Internal/TypeStructs.h"
#include "BlazeEngine/DataStructures/ByteStream.h"

namespace Blaze
{	
	extern std::vector<ResourceType> resourceTypes;

	BaseResource::BaseResource(ResourceTypeID typeID) 
		: scene(nullptr), typeID(typeID), id(-1)
	{
	}

	ResourceTypeID BaseResource::RegisterResourceType(Func<> construct, Func<> destruct, size_t size, size_t offset, Func<ByteStream&> serialize, Func<ByteStream&> deserialize)
	{
		resourceTypes.push_back({ construct, destruct, size, offset, serialize, deserialize});
		return resourceTypes.size() - 1;
	}
}