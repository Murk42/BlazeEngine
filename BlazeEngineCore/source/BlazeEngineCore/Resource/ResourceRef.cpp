#include "pch.h"
#include "BlazeEngineCore/Resource/ResourceRef.h"
#include "BlazeEngineCore/Resource/Resource.h"
#include "BlazeEngineCore/Debug/Logger.h"

namespace Blaze
{
	ResourceBaseRef::ResourceBaseRef()
		: resourceBase(nullptr)
	{
	}

	ResourceBaseRef::ResourceBaseRef(ResourceBase& resource)
		: resourceBase(&resource)
	{
		resource.IncreaseReferenceCount();
	}
	ResourceBaseRef::ResourceBaseRef(const ResourceBaseRef& other)
		: resourceBase(other.resourceBase)
	{
		if (resourceBase != nullptr)
			resourceBase->IncreaseReferenceCount();
	}
	ResourceBaseRef::ResourceBaseRef(ResourceBaseRef&& other) noexcept
		: resourceBase(other.resourceBase)
	{
		other.resourceBase = nullptr;
	}
	ResourceBaseRef::~ResourceBaseRef()
	{
		if (resourceBase != nullptr)
			resourceBase->DecreaseReferenceCount();
	}
	bool ResourceBaseRef::IsNull() const
	{
		return resourceBase == nullptr;
	}
	bool ResourceBaseRef::IsLoaded() const
	{
		return resourceBase == nullptr ? false : resourceBase->IsLoaded();
	}
	void ResourceBaseRef::WaitToLoad() const
	{
		if (resourceBase != nullptr)
			resourceBase->WaitToLoad();
	}
	uint32 ResourceBaseRef::Hash() const
	{
		return Blaze::Hash<void*>().Compute(resourceBase);
	}
	ResourceBaseRef::operator bool() const
	{
		return resourceBase != nullptr;
	}
	ResourceBase& ResourceBaseRef::GetResourceBase()
	{
		if (resourceBase == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Accessing a null resource base reference");

		return *resourceBase;
	}
	const ResourceBase& ResourceBaseRef::GetResourceBase() const
	{
		if (resourceBase == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Accessing a null resource base reference");

		return *resourceBase;
	}
	bool ResourceBaseRef::operator==(const ResourceBaseRef& other) const
	{
		return resourceBase == other.resourceBase;
	}
	bool ResourceBaseRef::operator!=(const ResourceBaseRef& other) const
	{
		return resourceBase != other.resourceBase;
	}
	ResourceBaseRef& ResourceBaseRef::operator=(const ResourceBaseRef& other)
	{
		if (resourceBase != nullptr)
			resourceBase->DecreaseReferenceCount();

		resourceBase = other.resourceBase;

		if (resourceBase != nullptr)
			resourceBase->IncreaseReferenceCount();

		return *this;
	}
	ResourceBaseRef& ResourceBaseRef::operator=(ResourceBaseRef&& other) noexcept
	{
		if (resourceBase != nullptr)
			resourceBase->DecreaseReferenceCount();

		resourceBase = other.resourceBase;
		other.resourceBase = nullptr;

		return *this;
	}
}