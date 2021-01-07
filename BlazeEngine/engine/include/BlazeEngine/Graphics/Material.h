#pragma once
#include "BlazeEngine/Core/Logger.h"
#include "BlazeEngine/DataStructures/OrderedTuple.h"
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
		using processed_type = std::conditional_t<std::is_base_of_v<Texture, T>, T*, T>;
	public:
		template<typename T>
		class Property
		{
		public:
			using Type = T;
			using StorageType = processed_type<T>;
		private:
			StorageType value;
			bool changed;
		public:

			const char* const name;

			Property(const char* name)
				: name(name), value()
			{

			}

			constexpr StorageType& operator= (const StorageType& v)
			{
				if (value != v)
					changed = true;
				value = v;
				return value;
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
		using TypesTuple = OrderedTuple<T...>;
		using PropertiesTuple = OrderedTuple<Property<T>...>;
		using Tuple = PropertiesTuple;

		constexpr PropertiesTuple& AsTuple() { return *(PropertiesTuple*)this; }
		constexpr const PropertiesTuple& AsTuple() const { return *(PropertiesTuple*)this; }
	};

	class BLAZE_API BaseMaterial
	{		
		BaseMaterial();
		BaseMaterial(const BaseMaterial&);
		BaseMaterial(BaseMaterial&&);

		void* GetProperty(StringView);
	protected:
		ShaderProgram sp;
	public:
		struct {
			BaseMaterialProperties::Property<Mat4f>* MVP;
			BaseMaterialProperties::Property<Mat4f>* model;
			BaseMaterialProperties::Property<Mat4f>* view;
			BaseMaterialProperties::Property<Mat4f>* projection;
			BaseMaterialProperties::Property<Texture2D>* mainTexture;
		} standardProperties;

		const ShaderProgram& GetShaderProgram() { return sp; }	

		template<typename T>
		BaseMaterialProperties::Property<T>* GetProperty(StringView name)
		{
			return (BaseMaterialProperties::Property<T>*)GetProperty(name);
		}

		virtual void UpdateShaderProgramUniforms() = 0;
		virtual void BindTextures() = 0;

		template<typename>
		friend class Material;
	};

	template<typename S>
	class Material : public BaseMaterial
	{				
		void* ptrs[S::TypesTuple::TupleSize];

		template<size_t index, typename T>
		constexpr bool IsCompatible(T& tuple)
		{
			if constexpr (index < S::TypesTuple::TupleSize - 1)
				return IsCompatible<index + 1>(tuple.nextTuple) && tuple.value.name == sp.GetUniforms()[index].GetName();
			else
				return tuple.value.name == sp.GetUniforms()[index].GetName();
		}

		template<size_t index, typename T>
		constexpr void SetPtrs(T& tuple)
		{
			ptrs[index] = &tuple.value;
			if constexpr (index < S::Tuple::TupleSize - 1)
				SetPtrs<index + 1>(tuple.nextTuple);
		}		

		template<size_t index, typename T>
		constexpr void UpdateShaderProgramUniforms(T& tuple, uint& textureIndex)
		{
			if (tuple.value.changed)
			{
				if constexpr (std::is_base_of_v<Texture, T::Type::Type>)
				{
					sp.SetUniform(&sp.GetUniforms()[index], &textureIndex);
					++textureIndex;
				}
				else
				{					
					sp.SetUniform(&sp.GetUniforms()[index], &tuple.value.value);
				}

				tuple.value.changed = false;
			}
			if constexpr (index != S::TypesTuple::TupleSize - 1)
				UpdateShaderProgramUniforms<index + 1, T::NextTuple>(tuple.nextTuple, textureIndex);
		}

		template<size_t index, typename T>
		constexpr void BindTextures(T& tuple, uint& textureIndex)
		{
			if constexpr (std::is_base_of_v<Texture, T::Type::Type>)
			{
				if (tuple.value.value != nullptr)
					tuple.value.value->Bind(textureIndex);
				++textureIndex;
			}
			if constexpr (index != S::TypesTuple::TupleSize - 1)
				BindTextures<index + 1, T::NextTuple>(tuple.nextTuple, textureIndex);
		}		

		inline void UpdateShaderProgramCompability()
		{
			if (sp.GetUniforms().size() != S::TypesTuple::TupleSize)
				Logger::AddLog(LogType::Warning, __FUNCTION__, "The material property structure isnt compatible with the given shaders. There is either too many or too few properties in the structure");
			else if (!IsCompatible<0>(properties.AsTuple()))
				Logger::AddLog(LogType::Warning, __FUNCTION__, "The material property structure isnt compatible with the given shaders. The property and uniforms names arent the same");
			else
			{
				auto& uniforms = sp.GetUniforms();

				for (int i = 0; i < uniforms.size(); ++i)
				{
					if (uniforms[i].GetName() == "u_MVP")
						standardProperties.MVP = (BaseMaterialProperties::Property<Mat4f>*)ptrs[i];
					else if (uniforms[i].GetName() == "u_model")
						standardProperties.model = (BaseMaterialProperties::Property<Mat4f>*)ptrs[i];
					else if (uniforms[i].GetName() == "u_view")
						standardProperties.view = (BaseMaterialProperties::Property<Mat4f>*)ptrs[i];
					else if (uniforms[i].GetName() == "u_projection")
						standardProperties.projection = (BaseMaterialProperties::Property<Mat4f>*)ptrs[i];
					else if (uniforms[i].GetName() == "u_texture")
						standardProperties.mainTexture = (BaseMaterialProperties::Property<Texture2D>*)ptrs[i];
				}
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
			uint textureIndex = 0;
			UpdateShaderProgramUniforms<0>(properties.AsTuple(), textureIndex);			
		}
		void BindTextures() override
		{
			uint textureIndex = 0;
			BindTextures<0>(properties.AsTuple(), textureIndex);
		}

		void Serialize(ByteStream& bs)
		{
			sp.Serialize(bs);
		}
		void Deserialize(ByteStream& bs)
		{
			sp.Deserialize(bs);		
			UpdateShaderProgramCompability();
		}

		bool Load(StringView path, bool emitWarningOnFail = true)
		{
			ByteStream bs;
			if (File::Read(path, &bs, emitWarningOnFail))
			{
				Deserialize(bs);
				return true;
			}
			return false;
		}
		bool Save(StringView path, bool emitWarningOnFail = true)
		{
			ByteStream bs;
			Serialize(bs);
			return File::Write(path, true, &bs, emitWarningOnFail);
		}		

		static_assert(std::is_base_of_v<BaseMaterialProperties, S>, "Material property struct must inherit from MaterialProperties");
		static_assert(sizeof(S) == sizeof(S::Tuple), "Material property struct template list doesnt match the struct members");
	};
}