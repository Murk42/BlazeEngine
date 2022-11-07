#pragma once
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/File/Path.h"

namespace Blaze::ResourceSystem
{
	class Resource
	{
		String name;
	protected:
		Path path;
	public:
		virtual void Load(Path path) { }
		virtual void Save(Path path) { }

		String GetName() const { return name; }
		Path GetPath() const { return path; }

		friend class ResourceStorage;
	};
}