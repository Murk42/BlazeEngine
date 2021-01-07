#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/GameSystem/ECS/Component.h"
#include "BlazeEngine/GameSystem/Resources/OldMesh.h"
#include "BlazeEngine/GameSystem/Resources/OldMaterial.h"

namespace Blaze
{
	class OldMesh;
	class OldMaterial;

	class BLAZE_API Model : public Component<Model>
	{
	public:
		OldMesh* mesh = nullptr;
		std::vector<OldMaterial*> materials;

		Model() = default;		
		Model(OldMesh* mesh, const std::initializer_list<OldMaterial*>& materials);		
	};
}