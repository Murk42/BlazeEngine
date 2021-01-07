#include "Model.h"

namespace Blaze
{
	Model::Model(OldMesh* mesh, const std::initializer_list<OldMaterial*>& materials)
		: mesh(mesh), materials(materials)
	{

	}
}