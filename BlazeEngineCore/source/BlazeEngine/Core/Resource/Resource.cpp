#include "pch.h"
#include "BlazeEngine/Core/Resource/Resource.h"

namespace Blaze
{
	ResourceBase::ResourceBase()
		: referenceCount(0)
	{		
		loaded.clear();
	}
	ResourceBase::~ResourceBase()
	{
		if (referenceCount != 0)
			BLAZE_LOG_FATAL("Destroying a resource with a reference count that isn't 0");
	}
	bool ResourceBase::IsLoaded() const
	{
		return loaded.test();
	}
	void ResourceBase::WaitToLoad() const
	{
		loaded.wait(false);
	}
	void ResourceBase::IncreaseReferenceCount()
	{
		++referenceCount;
	}
	void ResourceBase::DecreaseReferenceCount()
	{
		if (referenceCount == 0)
		{
			BLAZE_LOG_FATAL("Decreasing a reference count of a resource that has its resource count 0");
			return;
		}

		--referenceCount;

		if (referenceCount == 0)
			DestructResource();
	}
	uintMem ResourceBase::GetReferenceCount() const
	{
		return referenceCount;
	}
	void ResourceBase::MarkAsLoaded()
	{
		loaded.test_and_set();
		loaded.notify_all();
	}	
}