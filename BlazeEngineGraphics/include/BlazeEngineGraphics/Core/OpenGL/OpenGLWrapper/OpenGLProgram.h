#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"
#include "BlazeEngineCore/DataStructures/String.h"
#include "BlazeEngineCore/DataStructures/StringView.h"
#include "BlazeEngineCore/DataStructures/Array.h"
#include "BlazeEngineCore/DataStructures/ArrayView.h"
#include "BlazeEngineCore/Math/Vector.h"
#include "BlazeEngineCore/Math/Matrix.h"
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"

namespace Blaze::Graphics::OpenGLWrapper
{
	class Shader;
	class GraphicsBuffer;	

	class BLAZE_GRAPHICS_API ShaderProgram
	{		
	public:
		ShaderProgram();
		ShaderProgram(uint id);
		ShaderProgram(const ShaderProgram&) = delete;
		ShaderProgram(ShaderProgram&&) noexcept;
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