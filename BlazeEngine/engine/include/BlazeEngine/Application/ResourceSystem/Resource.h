#pragma once
#include "BlazeEngine/File/Path.h"

namespace Blaze::ResourceSystem
{
#define RESOURCE(name)								\
	static constexpr const char* typeName = #name;	

	class BLAZE_API Resource
	{
		String name;
	protected:
		Path path;
	public:
		virtual Result Load(Path path) { return Result(); }
		virtual Result Save(Path path) { return Result(); }

		String GetName() const { return name; }
		Path GetPath() const { return path; }

		friend class ResourceStorage;
	};
}