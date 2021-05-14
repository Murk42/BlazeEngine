#pragma once
#include <vector>
#include <cstdarg>
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/DataStructures/StringView.h"
#include "BlazeEngine/Graphics/Shader.h"
#include <utility>
#include "Buffer.h"

namespace Blaze
{	
	class ByteStream;

	class BLAZE_API Uniform
	{
		String name;		
		Type type;
		int pos;	
		int count;		
		bool sampler;

		Uniform(const Uniform&) = delete;
		void operator=(const Uniform&) = delete;
		void operator=(Uniform&&) = delete;
	public:			
		Uniform();		
		Uniform(Uniform&&) noexcept;
		~Uniform();

		inline Type GetType() const { return type; }
		inline String GetName() const { return name; }
		inline bool IsSampler() const { return sampler; }		

		friend class ShaderProgram;		
	};

	struct BLAZE_API UniformBlock
	{			
		struct Member
		{
			Uniform u;
			int offset = 0;
		};

		String name;
		int index;
		int size;		
		std::vector<Member> members;
		void* data;
		Buffer buffer;

		UniformBlock(const UniformBlock&) = delete;
		void operator=(const UniformBlock&) = delete;
		void operator=(UniformBlock&&) = delete;
	public:		
		UniformBlock();
		UniformBlock(UniformBlock&&) noexcept;
		~UniformBlock();		

		friend class ShaderProgram;				
	};

	class BLAZE_API ShaderProgram
	{		
		std::vector<Uniform> uniforms;				
		unsigned id;			

		void GatherUniformData();
	public:
		ShaderProgram();		
		ShaderProgram(const ShaderProgram&);
		ShaderProgram(ShaderProgram&&) noexcept;
		~ShaderProgram();		

		void Bind() const;
		static void Unbind();
		static ShaderProgram* GetBound();
		
		bool SetShaders(const std::initializer_list<const Shader*>& shaders);					

		const Uniform* GetUniform(const String& name) const;
		inline const std::vector<Uniform>& GetUniforms() const { return uniforms; }

		static UniformBlock* GetUniformBlock(const String& name);
		static const std::vector<UniformBlock>& GetUniformBlocks();

		void SetUniform(const Uniform* uniform, const void* ptr) const;
		template<typename T>
		void SetUniformValue(const Uniform* uniform, const T& value) const
		{
			SetUniform(uniform, &value);
		}
		bool SetUniform(StringView name, const void* ptr) const;		
		template<typename T>
		void SetUniformValue(StringView name, const T& value)
		{
			SetUniform(name, &value);
		}

		void Serialize(ByteStream&);
		void Deserialize(ByteStream&);
		static void SetUniformBlock(const UniformBlock* data, const void* ptr);
		static bool SetUniformBlock(const String& name, const void* ptr);

		void operator=(const ShaderProgram&);
		void operator=(ShaderProgram&&) noexcept;
	};
}