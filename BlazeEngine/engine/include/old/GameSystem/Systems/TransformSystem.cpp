#include "TransformSystem.h"

namespace Blaze
{
	void TransformSystem::Update(Array<Transform> transforms)
	{
		for (auto& t : transforms)
			t.UpdateMatrix();		
	}
}
