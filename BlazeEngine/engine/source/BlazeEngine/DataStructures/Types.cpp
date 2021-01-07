#include "BlazeEngine/DataStructures/Types.h"
#include <ctype.h>

namespace Blaze
{	
	Type GetType(const String& c)
	{
		String s = c;
		constexpr static std::pair<const char*, Type> values[]{
			{ "vec2b",         Type::Vec2b         },
			{ "vec2u",         Type::Vec2u         },
			{ "vec2i",         Type::Vec2i         },
			{ "vec2f",         Type::Vec2f         },
			{ "vec2d",         Type::Vec2d         },

			{ "vec3b",         Type::Vec3b         },
			{ "vec3u",         Type::Vec3u         },
			{ "vec3i",         Type::Vec3i         },
			{ "vec3f",         Type::Vec3f         },
			{ "vec3d",         Type::Vec3d         },

			{ "vec4b",         Type::Vec4b         },
			{ "vec4u",         Type::Vec4u         },
			{ "vec4i",         Type::Vec4i         },
			{ "vec4f",         Type::Vec4f         },
			{ "vec4d",         Type::Vec4d         },

			{ "mat2f",         Type::Mat2f         },
			{ "mat2d",         Type::Mat2d         },

			{ "mat3f",         Type::Mat3f         },
			{ "mat3d",         Type::Mat3d         },

			{ "mat4f",         Type::Mat4f         },
			{ "mat4d",         Type::Mat4d         },

			{ "bool",          Type::Bool          },
			{ "byte",          Type::Byte          },
			{ "unsignedbyte",  Type::UnsignedByte  },
			{ "short",         Type::Short         },
			{ "unsignedshort", Type::UnsignedShort },
			{ "int",           Type::Int           },
			{ "unsignedint",   Type::UnsignedInt   },
			{ "float",         Type::Float         },
			{ "double",        Type::Double        }
		};
		for (int i = 0; i < s.Size(); i++)
			s[i] = (char)tolower(s[i]);

		for (int i = 0; i < _countof(values); i++)
			if (values[i].first == s)
				return values[i].second;				
		return (Type)0;
	}	
}