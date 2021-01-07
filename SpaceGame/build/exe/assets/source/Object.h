#pragma once
#define BLAZE_ONLY_CORE
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

#include "Transform.h"

class Object
{
public:
	inline static vector<Object*> allObjects;
	Renderable* renderable;
	Transform tran;

	Object()
		: renderable(nullptr)
	{
		allObjects.push_back(this);
	}
	Object(Renderable* renderable, const Transform& tran)
		: renderable(renderable), tran(tran)
	{
		allObjects.push_back(this);
	}
	~Object()
	{
		auto i = std::find(allObjects.begin(), allObjects.end(), this);
		allObjects.erase(i);
	}

	virtual void Update()
	{
		tran.UpdateMatrix();
	}

	static void UpdateAllObjects()
	{
		for (auto& o : allObjects)
			o->Update();
	}
};