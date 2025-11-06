#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/String/String.h"
#include "BlazeEngine/Core/String/StringView.h"
#include "BlazeEngine/Core/Container/Array.h"
#include "BlazeEngine/Core/Container/ArrayView.h"
#include "BlazeEngine/Core/Math/Vector.h"
#include "BlazeEngine/Core/Math/Matrix.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"

namespace Blaze::Graphics::OpenGL
{
	class Shader;
	class GraphicsBuffer;

	class BLAZE_API ShaderProgram
	{
	public:
		ShaderProgram();
		ShaderProgram(uint id);
		ShaderProgram(const ShaderProgram&) = delete;
		ShaderProgram(ShaderProgram&&) noexcept;
		ShaderProgram(ArrayView<Shader&> shaders);
		~ShaderProgram();

		void AttachShader(const Shader& shader);
		void DetachShader(const Shader& shader);
		void LinkProgram();

		void LinkShaders(ArrayView<Shader&> shaders);

		uint GetUniformCount() const;
		//Returns -1 if 'name' is not the name of an active uniform
		int GetUniformLocation(const StringView& name);
		void GetUniformData(uint index, String& name, uint& size, UniformType& type) const;

		uint GetUniformBlockCount() const;
		void GetUniformBlockData(uint index, int& location, String& name, uint& size, Array<int>& memberLocations) const;
		void GetUniformBlockMemberData(int location, String& name, uint& size, UniformType& type, uint& offset);
		void BindUniformBlock(int location, uint binding);

		void SetUniform(int location, const int& value);
		void SetUniform(int location, const uint& value);
		void SetUniform(int location, const float& value);
		void SetUniform(int location, const double& value);
		void SetUniform(int location, const Vec2i& value);
		void SetUniform(int location, const Vec2f& value);
		void SetUniform(int location, const Vec2d& value);
		void SetUniform(int location, const Vec3i& value);
		void SetUniform(int location, const Vec3f& value);
		void SetUniform(int location, const Vec3d& value);
		void SetUniform(int location, const Vec4i& value);
		void SetUniform(int location, const Vec4f& value);
		void SetUniform(int location, const Vec4d& value);
		void SetUniform(int location, const Mat2f& value);
		void SetUniform(int location, const Mat3f& value);
		void SetUniform(int location, const Mat4f& value);

		ShaderProgramState GetState() const { return state; }

		[[nodiscard]]
		uint ReleaseHandleOwnership();
		void Release();
		uint GetHandle() const { return id; }

		ShaderProgram& operator=(const ShaderProgram&) = delete;
		ShaderProgram& operator=(ShaderProgram&&) noexcept;
	private:
		ShaderProgramState state;
		uint id;
	};
}