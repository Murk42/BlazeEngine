#pragma once
#include "BlazeEngine/Core/Logger.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/Graphics/OpenGL/Shader.h"
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/DataStructures/Matrix.h"

namespace Blaze
{
	namespace OpenGL
	{
		enum class UniformType
		{
			Mat4,
			Mat3,
			Mat2,
			Vec4,
			Vec3,
			Vec2,
			Sampler2D,			
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
						String log = GetLinkingLog();
						Logger::AddLog(LogType::Error, "Program linking", std::move(log));
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

			inline void SetUniform(int location, const int& value);
			inline void SetUniform(int location, const Vec4f& value);
			inline void SetUniform(int location, const Mat4f& value);

			inline void Use() const;
			static inline void Unuse();
			static inline Program* GetUsed();

			inline Program& operator=(const Program&) = delete;
			inline Program& operator=(Program&&) noexcept;			
		};
	}
}