#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Core/Result.h"
#include "BlazeEngine/Logging/Logger.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/Graphics/OpenGL/OpenGLShader.h"
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/DataStructures/Matrix.h"
#include <vector>

namespace Blaze
{
	namespace OpenGL
	{
		enum class UniformType
		{
			Mat4 = 35676,
			Mat3 = 35675,
			Mat2 = 35674,
			Vec4 = 35666,
			Vec3 = 35665,
			Vec2 = 35664,
			Sampler2D = 35678,
		};
		enum class ShaderProgramState
		{
			Invalid,
			Valid,
		};

		class BLAZE_API ShaderProgram
		{
			ShaderProgramState state;
			uint id;
			uint maxUniformNameLenght;
			uint maxUniformBlockNameLenght;

		public:
			ShaderProgram();
			ShaderProgram(const ShaderProgram&) = delete;
			ShaderProgram(ShaderProgram&&) noexcept;
			~ShaderProgram();

			void AttachShader(const Shader& shader);
			void DetachShader(const Shader& shader);
			Result LinkProgram();
			String GetLinkingLog();
					
			Result LinkShaders(const std::initializer_list<Shader*>& shaders);

			uint GetUniformCount() const;	
			int GetUniformLocation(const StringView& name);
			void GetUniformData(uint index, String& name, uint& size, UniformType& type) const;

			uint GetUniformBlockCount() const;
			void GetUniformBlockData(uint index, int& location, String& name, uint& size, std::vector<int>& memberLocations) const;
			void GetUniformBlockMemberData(int location, String& name, uint& size, UniformType& type, uint& offset);
				
			void SetUniform(int location, const bool& value);
			void SetUniform(int location, const int& value);
			void SetUniform(int location, const uint& value);
			void SetUniform(int location, const float& value);			
			void SetUniform(int location, const Vec2i& value);
			void SetUniform(int location, const Vec2f& value);
			void SetUniform(int location, const Vec3i& value);
			void SetUniform(int location, const Vec3f& value);
			void SetUniform(int location, const Vec4f& value);
			void SetUniform(int location, const Mat2f& value);
			void SetUniform(int location, const Mat3f& value);
			void SetUniform(int location, const Mat4f& value);

			uint GetHandle() const { return id; }
			ShaderProgramState GetState() const { return state; }

			ShaderProgram& operator=(const ShaderProgram&) = delete;
			ShaderProgram& operator=(ShaderProgram&&) noexcept;			
		};
	}
}