#pragma once
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"

namespace Blaze::Graphics::OpenGLWrapper
{
	class Shader;
	class GraphicsBuffer;	

	class BLAZE_GRAPHICS_API ShaderProgram
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
		void LinkProgram();

		void LinkShaders(const std::initializer_list<Shader*>& shaders);

		uint GetUniformCount() const;
		int GetUniformLocation(const StringView& name);
		void GetUniformData(uint index, String& name, uint& size, UniformType& type) const;

		uint GetUniformBlockCount() const;
		void GetUniformBlockData(uint index, int& location, String& name, uint& size, std::vector<int>& memberLocations) const;
		void GetUniformBlockMemberData(int location, String& name, uint& size, UniformType& type, uint& offset);
		void BindUniformBlock(int location, uint binding);

		void SetUniform(int location, const int& value) const;
		void SetUniform(int location, const uint& value) const;
		void SetUniform(int location, const float& value) const;
		void SetUniform(int location, const double& value) const;
		void SetUniform(int location, const Vec2i& value) const;
		void SetUniform(int location, const Vec2f& value) const;
		void SetUniform(int location, const Vec2d& value) const;
		void SetUniform(int location, const Vec3i& value) const;
		void SetUniform(int location, const Vec3f& value) const;
		void SetUniform(int location, const Vec3d& value) const;
		void SetUniform(int location, const Vec4i& value) const;
		void SetUniform(int location, const Vec4f& value) const;
		void SetUniform(int location, const Vec4d& value) const;
		void SetUniform(int location, const Mat2f& value) const;
		void SetUniform(int location, const Mat3f& value) const;
		void SetUniform(int location, const Mat4f& value) const;

		uint GetHandle() const { return id; }
		ShaderProgramState GetState() const { return state; }

		ShaderProgram& operator=(const ShaderProgram&) = delete;
		ShaderProgram& operator=(ShaderProgram&&) noexcept;
	};
}