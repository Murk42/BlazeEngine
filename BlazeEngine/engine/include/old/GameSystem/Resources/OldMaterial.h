#pragma once
#include <vector>
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Graphics/ShaderProgram.h"
#include "BlazeEngine/Graphics/Shader.h"
#include "BlazeEngine/Core/Logger.h"
#include "BlazeEngine/Graphics/Texture.h"
#include "BlazeEngine/DataStructures/StringView.h"
#include "BlazeEngine/GameSystem/Resource.h"

namespace Blaze
{	
	typedef uint PropertyID;
	class BLAZE_API OldMaterial : public Resource<OldMaterial>
	{		
		ShaderProgram shaderProgram;		
		void* propertyValues;		

		void _SetProperty(PropertyID, const void*);		
	public:
		OldMaterial();
		OldMaterial(const OldMaterial&);
		OldMaterial(OldMaterial&&);		
		OldMaterial(const std::initializer_list<const Shader*>& shaders);
		~OldMaterial();

		inline ShaderProgram& GetShaderProgram() { return shaderProgram; }
		inline const ShaderProgram& GetShaderProgram() const { return shaderProgram; }
		uint GetPropertyID(StringView name);

		void SetShaders(const std::initializer_list<const Shader*>& shaders);

		template<typename T>
		inline void SetProperty(PropertyID id, const T& value)
		{
			_SetProperty(id, (const void*)&value);						
		}				

		void UpdateUniforms();		
		void BindTextures();		

		void operator=(const OldMaterial&);
		void operator=(OldMaterial&&);
	};
}