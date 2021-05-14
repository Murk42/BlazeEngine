#pragma once
#include "BlazeEngine/Core/Logger.h"
#include "BlazeEngine/DataStructures/Tuple.h"
#include "BlazeEngine/Graphics/ShaderProgram.h"
#include "BlazeEngine/Graphics/Texture.h"
#include "BlazeEngine/Utilities/File.h"

namespace Blaze
{
	template<typename>
	class Material;

	class BaseMaterialProperties
	{
		template<typename T>
		using processed_type = std::conditional_t<std::is_base_of_v<Texture, T>,const T*, T>;
	public:
		template<typename T>
		class Property
		{
		public:
			using Type = T;
			using StorageType = processed_type<T>;
		private:
			uint uniformIndex;
			bool changed;
			StorageType value;
		public:

			const char* const name;

			Property(const char* name)
				: name(name), value(), changed(true)
			{

			}

			constexpr Property<T>& operator= (const StorageType& v)
			{
				if (value != v)
				{
					changed = true;
					value = v;
				}
				return *this;
			}
			
			constexpr const StorageType& GetValue() const { return value; }

			template<typename>
			friend class Material;
		};
	};

	template<typename ... T>
	class MaterialProperties : public BaseMaterialProperties
	{
	public:
		using TemplateGroup =TemplateGroup<T...>;
		using PropertiesTuple = Tuple<Property<T>...>;		

		constexpr PropertiesTuple& AsTuple() { return *(PropertiesTuple*)this; }
		constexpr const PropertiesTuple& AsTuple() const { return *(PropertiesTuple*)this; }
	};

	class BLAZE_API BaseMaterial
	{		

		BaseMaterial();
		BaseMaterial(const BaseMaterial&);
		BaseMaterial(BaseMaterial&&);

		void* GetProperty(StringView);
		const void* GetProperty(StringView) const;
	protected:
		bool valid = false;
		ShaderProgram sp;
	public:
		struct {
			BaseMaterialProperties::Property<Mat4f>* MVP;			
			BaseMaterialProperties::Property<Texture>* texture;
			BaseMaterialProperties::Property<Vec4f>* color;
		} standardProperties;

		const ShaderProgram& GetShaderProgram() { return sp; }	

		template<typename T>
		BaseMaterialProperties::Property<T>* GetProperty(StringView name)
		{
			if (valid)
				return (BaseMaterialProperties::Property<T>*)GetProperty(name);
			else
			{
				Logger::AddLog(LogType::Warning, __FUNCTION__, "Using an invalid material");
				return nullptr;
			}
		}
		template<typename T>
		const BaseMaterialProperties::Property<T>* GetProperty(StringView name) const
		{
			if (valid)
				return (BaseMaterialProperties::Property<T>*)GetProperty(name);
			else
			{
				Logger::AddLog(LogType::Warning, __FUNCTION__, "Using an invalid material");
				return nullptr;
			}
		}

		virtual void UpdateShaderProgramUniforms() = 0;
		virtual void BindTextures() = 0;

		bool IsValid() const { return valid; }

		template<typename>
		friend class Material;
	};

	template<typename S>
	class Material : public BaseMaterial
	{				
		void* ptrs[S::TemplateGroup::Size];

		template<size_t index, typename T>
		constexpr bool IsCompatible(T& tuple)
		{
			auto& uniforms = sp.GetUniforms();
			bool found = false;

			for (int i = 0; i < uniforms.size(); ++i)
				if (tuple.value.name == uniforms[i].GetName())
				{
					found = true;
					tuple.value.uniformIndex = i;						
					break;
				}

			if constexpr (index < S::TemplateGroup::Size - 1)			
				if (found)
					return IsCompatible<index + 1>(tuple.nextTuple);
				else
					return  false;			
			else			
				return found;						
		}

		template<size_t index, typename T>
		constexpr void SetPtrs(T& tuple)
		{
			ptrs[index] = &tuple.value;
			if constexpr (T::TemplateGroup::Size > 1)
				SetPtrs<index + 1>(tuple.nextTuple);
		}		

		template<size_t index, typename T>
		constexpr void UpdateShaderProgramUniforms(T& tuple, uint& textureIndex)
		{
			if (tuple.value.changed)
			{
				if constexpr (std::is_base_of_v<Texture, T::TemplateGroup::First::Type>)
				{
					sp.SetUniform(&sp.GetUniforms()[tuple.value.uniformIndex], &textureIndex);
					++textureIndex;
				}
				else
				{					
					sp.SetUniform(&sp.GetUniforms()[tuple.value.uniformIndex], &tuple.value.value);
				}

				tuple.value.changed = false;
			}
			if constexpr (index != S::TemplateGroup::Size - 1)
				UpdateShaderProgramUniforms<index + 1>(tuple.nextTuple, textureIndex);
		}

		template<size_t index, typename T>
		constexpr void BindTextures(T& tuple, uint& textureIndex)
		{
			if constexpr (std::is_base_of_v<Texture, T::TemplateGroup::First::Type>)
			{
				if (tuple.value.value != nullptr)
					tuple.value.value->Bind(textureIndex);
				++textureIndex;
			}
			if constexpr (index != S::TemplateGroup::Size - 1)
				BindTextures<index + 1>(tuple.nextTuple, textureIndex);
		}		

		template<typename T>
		constexpr void SetStandardProperty(T& tuple, const std::vector<Uniform>& uniforms)
		{
			const Uniform& uniform = uniforms[tuple.value.uniformIndex];
			if (uniform.GetName() == "u_MVP" && std::is_same_v<T::TemplateGroup::First::Type, Mat4f>)
				standardProperties.MVP = (BaseMaterialProperties::Property<Mat4f>*) & tuple.value;
			else if (uniform.GetName() == "u_texture" && uniform.IsSampler() && std::is_base_of_v<Texture, T::TemplateGroup::First::Type>)
				standardProperties.texture = (BaseMaterialProperties::Property<Texture>*) & tuple.value;
			else if (uniform.GetName() == "u_color" && std::is_same_v<T::TemplateGroup::First::Type, Vec4f>)
				standardProperties.color = (BaseMaterialProperties::Property<Vec4f>*) & tuple.value;

			if constexpr (T::TemplateGroup::Size > 1)
				SetStandardProperty(tuple.nextTuple, uniforms);
		}

		inline void UpdateShaderProgramCompability()
		{
			if (sp.GetUniforms().size() != S::TemplateGroup::Size)
			{
				Logger::AddLog(LogType::Warning, __FUNCTION__, "The material property structure isnt compatible with the given shaders. There is either too many or too few properties in the structure");
				valid = false;
			}
			else if (!IsCompatible<0>(properties.AsTuple()))
			{
				Logger::AddLog(LogType::Warning, __FUNCTION__, "The material property structure isnt compatible with the given shaders. The property and uniforms names arent the same");
				valid = false;
			}
			else
			{
				SetStandardProperty(properties.AsTuple(), sp.GetUniforms());				
				valid = true;
			}
		}
	public:
		S properties;				

		Material()			
		{			
			SetPtrs<0>(properties.AsTuple());
		}
		Material(const Material& mat)
			: BaseMaterial(mat), properties(mat.properties)
		{		
			SetPtrs<0>(properties.AsTuple());			
		}
		Material(Material&& mat)
			: BaseMaterial(std::move(mat)), properties(std::move(mat.properties))
		{			
			SetPtrs<0>(properties.AsTuple());
		}
		template<typename ... T>
		Material(const T& ... shaders)
			: BaseMaterial(&properties)
		{
			SetPtrs<0>(properties.AsTuple());
			SetShaders(shaders...);
		}
		Material(const std::initializer_list<const Shader*>& shaders)
			: BaseMaterial(&properties)
		{
			SetPtrs<0>(properties.AsTuple());
			SetShaders(shaders);
		}

		template<typename ... T>
		constexpr void SetShaders(const T& ... shaders)
		{
			SetShaders({ &shaders... });
		}
		void SetShaders(const std::initializer_list<const Shader*>& shaders)
		{
			if (sp.SetShaders(shaders))			
				UpdateShaderProgramCompability();			
		}

		void UpdateShaderProgramUniforms() override
		{		
			if (valid)
			{
				uint textureIndex = 0;
				UpdateShaderProgramUniforms<0>(properties.AsTuple(), textureIndex);
			}
			else			
				Logger::AddLog(LogType::Warning, __FUNCTION__, "Using an invalid material");							
		}
		void BindTextures() override
		{
			if (valid)
			{
				uint textureIndex = 0;
				BindTextures<0>(properties.AsTuple(), textureIndex);
			}
			else
				Logger::AddLog(LogType::Warning, __FUNCTION__, "Using an invalid material");			
		}

		void Serialize(ByteStream& bs)
		{
			if (valid)
				sp.Serialize(bs);
			else			
				Logger::AddLog(LogType::Warning, __FUNCTION__, "Using an invalid material");				
		}
		void Deserialize(ByteStream& bs)
		{
			sp.Deserialize(bs);		
			UpdateShaderProgramCompability();
		}

		bool Load(StringView path, bool emitWarningOnFail = true)
		{
			if (valid)
			{
				ByteStream bs;
				if (File::Read(path, &bs, emitWarningOnFail))
				{
					Deserialize(bs);
					return true;
				}
				return false;
			}
			else
			{
				Logger::AddLog(LogType::Warning, __FUNCTION__, "Using an invalid material");
				return false;
			}
		}
		bool Save(StringView path, bool emitWarningOnFail = true)
		{
			ByteStream bs;
			Serialize(bs);
			return File::Write(path, true, &bs, emitWarningOnFail);
		}		

		static_assert(std::is_base_of_v<BaseMaterialProperties, S>, "Material property struct must inherit from MaterialProperties");
		static_assert(sizeof(S) == sizeof(S::PropertiesTuple), "Material property struct template list doesnt match the struct members");
	};
}