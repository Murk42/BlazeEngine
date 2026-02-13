#include "pch.h"
#include "BlazeEngine/Core/Resource/ResourceRef.h"
#include "BlazeEngine/Core/Debug/Logger.h"

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
	ResourceBase* ResourceBaseRef::GetResource() const
	{
		return resourceBase;
	}
	ResourceBaseRef::operator bool() const
	{
		return resourceBase != nullptr;
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