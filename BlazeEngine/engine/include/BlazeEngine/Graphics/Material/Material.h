#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/Graphics/GraphicsLibrary.h"
#include "BlazeEngine/Core/Logger.h"

namespace Blaze
{
	namespace Renderer
	{
		BLAZE_API void SetActiveTextureSlot(uint slot);
		BLAZE_API void SelectTexture(Graphics::Core::Texture2D*);		
		BLAZE_API void SelectTexture(Graphics::Core::Texture2DArray*);		
		BLAZE_API void SelectTexture(Graphics::Core::TextureCubemap*);		
	}

	class BLAZE_API Material
	{
		static constexpr uint maxProperties = 16;

		class BLAZE_API PropertyBase
		{
			Graphics::Core::UniformType type;
			int location = -1;
			const char* name = nullptr;

			virtual void SetProperty(Graphics::Core::Program*, int) = 0;
		public:
			PropertyBase(const char* name);

			int GetLocation() const { return location; }
			const char* GetName() const { return name; }

			friend Material;
		};
	public:

		template<typename T>
		class Property : public PropertyBase
		{
			static constexpr bool isTexture = std::is_same_v<Graphics::Core::Texture2D, T> ||
				std::is_same_v<Graphics::Core::Texture2DArray, T> ||
				std::is_same_v<Graphics::Core::TextureCubemap, T>;
			using UniformType = std::conditional_t<isTexture, int, T>;

			void SetProperty(Graphics::Core::Program* program, int slot) override
			{
				if constexpr (isTexture)
				{
					program->SetUniform<UniformType>(GetLocation(), slot);
					if (value == nullptr)
						BLAZE_WARNING_LOG("Blaze Engine", "No texture was set");
					else
					{
						::Blaze::Renderer::SetActiveTextureSlot(slot);
						::Blaze::Renderer::SelectTexture(value);
					}
				}
				else
					program->SetUniform<UniformType>(GetLocation(), value);
			}

			using ValueType = std::conditional_t<isTexture, T*, T>;
			ValueType value;
		public:

			Property(const char* name, ValueType value = ValueType())
				: PropertyBase(name), value(value) { }

			Property& operator=(const ValueType& value)
			{
				this->value = value;
				return *this;
			}
		};
	private:
		Graphics::Core::Program* program;

		uint propertyCount;
		PropertyBase* properties[maxProperties];		
	public:		
		Material();

		Graphics::Core::Program* GetProgram() { return program; }
		const Graphics::Core::Program* GetProgram() const { return program; }
		void SetProgram(Graphics::Core::Program* program);
		void UpdateProperties();		
	};
}