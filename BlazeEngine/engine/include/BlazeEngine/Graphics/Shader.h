#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/DataStructures/Common.h"

namespace Blaze
{	
	enum class ShaderType
	{
		FragmentShader = 0x8B30,
		VertexShader = 0x8B31,
		GeometryShader = 0x8DD9,
	};

	class BLAZE_API Shader
	{
		uint id;
		ShaderType type;
		String path;

		Shader(const Shader&) = delete;
		void operator=(const Shader&) = delete;
	public:		
		Shader(ShaderType);
		Shader(ShaderType, const StringView&);
		Shader(Shader&&) noexcept;
		~Shader();

		bool LoadShader(const StringView& path);
		bool SetSource(char* data, size_t size);

		inline ShaderType GetType() const { return type; }
		inline String GetPath() const { return path; }

		void operator=(Shader&&) noexcept;

		friend class ShaderProgram;
	};	
}