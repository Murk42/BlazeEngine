#include "BlazeEngine/Graphics/Material.h"

namespace Blaze
{
	void* BaseMaterial::GetProperty(StringView name)
	{
		const auto& uniforms = sp.GetUniforms();

		for (int i = 0; i < uniforms.size(); ++i)
			if (uniforms[i].GetName() == name)
				return ((void**)(this + 1))[i];

		Logger::AddLog(LogType::Warning, __FUNCTION__, "Couldnt find property with name \"" + name + "\"");
		return nullptr;
	}

	BaseMaterial::BaseMaterial()		
	{

	}
	BaseMaterial::BaseMaterial(const BaseMaterial& mat)
		: sp(mat.sp)
	{

	}
	BaseMaterial::BaseMaterial(BaseMaterial&& mat)
		: sp(std::move(mat.sp))
	{

	}
}