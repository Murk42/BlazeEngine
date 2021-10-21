#pragma once
#include "BlazeEngine/Core/Logger.h"
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

		class BLAZE_API Program
		{
			uint id;
			uint maxUniformNameLenght;
			uint maxUniformBlockNameLenght;

		public:
			inline Program();
			inline Program(const Program&) = delete;
			inline Program(Program&&) noexcept;
			inline ~Program();

			inline void AttachShader(const Shader& shader);
			inline void DetachShader(const Shader& shader);
			inline bool LinkProgram();
			inline String GetLinkingLog();
					
			inline bool LinkShaders(const std::initializer_list<Shader*>& shaders, bool emitLogOnError =  true)
			{
				for (auto& s : shaders)
					AttachShader(*s);

				if (LinkProgram() == BLAZE_ERROR)
				{
					if (emitLogOnError)
					{
						String message = GetLinkingLog();
						BLAZE_ERROR_LOG("Blaze Engine", std::move(message));
					}

					return BLAZE_ERROR;
				}

				for (auto& s : shaders)
					DetachShader(*s);

				return BLAZE_OK;
			}

			inline uint GetUniformCount() const;	
			inline int GetUniformLocation(const StringView& name);
			inline void GetUniformData(uint index, String& name, uint& size, UniformType& type) const;

			inline uint GetUniformBlockCount() const;
			inline void GetUniformBlockData(uint index, int& location, String& name, uint& size, std::vector<int>& memberLocations) const;
			inline void GetUniformBlockMemberData(int location, String& name, uint& size, UniformType& type, uint& offset);

			template<typename T>
			void SetUniform(int, const T&);
			template<> void SetUniform(int location, const int& value);
			template<> void SetUniform(int location, const float& value);			
			template<> void SetUniform(int location, const Vec3f& value);
			template<> void SetUniform(int location, const Vec4f& value);
			template<> void SetUniform(int location, const Mat4f& value);

			inline uint GetHandle() const { return id; }

			inline Program& operator=(const Program&) = delete;
			inline Program& operator=(Program&&) noexcept;			
		};

		class ProgramUniforms
		{

		};
	}
}