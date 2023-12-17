#pragma once
#include "BlazeEngineCore/Graphics/OpenGL/OpenGLShader.h"
#include "BlazeEngineCore/Math/Matrix.h"

namespace Blaze
{
	namespace OpenGL
	{
		class GraphicsBuffer;

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
			UnsuccesfullyLinked,
			SuccesfullyLinked,			
		};

		class BLAZE_CORE_API ShaderProgram
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

			Result AttachShader(const Shader& shader);
			Result DetachShader(const Shader& shader);
			Result LinkProgram();			
					
			Result LinkShaders(const std::initializer_list<Shader*>& shaders);

			uint GetUniformCount() const;	
			int GetUniformLocation(const StringView& name);
			Result GetUniformData(uint index, String& name, uint& size, UniformType& type) const;

			uint GetUniformBlockCount() const;
			Result GetUniformBlockData(uint index, int& location, String& name, uint& size, std::vector<int>& memberLocations) const;
			Result GetUniformBlockMemberData(int location, String& name, uint& size, UniformType& type, uint& offset);
			Result BindUniformBlock(int location, uint binding);
							
			Result SetUniform(int location, const int& value) const;
			Result SetUniform(int location, const uint& value) const;
			Result SetUniform(int location, const float& value) const;		
			Result SetUniform(int location, const double& value) const;
			Result SetUniform(int location, const Vec2i& value) const;
			Result SetUniform(int location, const Vec2f& value) const;
			Result SetUniform(int location, const Vec2d& value) const;
			Result SetUniform(int location, const Vec3i& value) const;
			Result SetUniform(int location, const Vec3f& value) const;
			Result SetUniform(int location, const Vec3d& value) const;
			Result SetUniform(int location, const Vec4i& value) const;
			Result SetUniform(int location, const Vec4f& value) const;
			Result SetUniform(int location, const Vec4d& value) const;
			Result SetUniform(int location, const Mat2f& value) const;
			Result SetUniform(int location, const Mat3f& value) const;
			Result SetUniform(int location, const Mat4f& value) const;

			uint GetHandle() const { return id; }
			ShaderProgramState GetState() const { return state; }

			ShaderProgram& operator=(const ShaderProgram&) = delete;
			ShaderProgram& operator=(ShaderProgram&&) noexcept;			
		};
	}
}