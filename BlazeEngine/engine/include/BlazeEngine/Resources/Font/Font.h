#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/DataStructures/StringViewUTF8.h"
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/DataStructures/Buffer.h"
#include "BlazeEngine/Graphics/GraphicsLibrary.h"
#include <vector>
#include <unordered_map>

namespace Blaze
{		
	enum class FontType
	{
		Monochrome,
		Antialiased,		
	};

	class BLAZE_API Font
	{
	public:
		struct CharacterData
		{
			int character;
			Vec2f uv1;
			Vec2f uv2;
			Vec2i size;
			Vec2i offset;
			uint advance;
		};

		struct CharacterVertex
		{
			Vec2f p1;
			Vec2f p2;
			Vec2f uv1;
			Vec2f uv2;					
		};
	private:
		void* ptr;						
		std::unordered_map<uint, CharacterData> characterMap;
		Graphics::Core::Texture2D texture;
		FontType type;
		uint resolution;
		uint height;
	public:
		Font();		
		~Font();		

		bool GetCharacterData(UnicodeChar character, CharacterData& data) const;
		CharacterVertex* GenerateVertices(StringViewUTF8 text, uint& vertexCount, Vec2f& size) const;
		
		int Load(StringView path, FontType type, uint resolution, bool emitLogOnFail = true);

		inline Graphics::Core::Texture2D& GetTexture() { return texture; }
		inline FontType GetType() const { return type; }
		inline uint GetHeight() const { return height; }
	};	
}