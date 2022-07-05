#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/DataStructures/StringViewUTF8.h"
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/DataStructures/Buffer.h"
#include "BlazeEngine/DataStructures/Rect.h"
#include "BlazeEngine/Graphics/GraphicsLibrary.h"
#include <vector>
#include <unordered_map>

namespace Blaze
{		
	class Font;

	struct CharacterData
	{
		float advance;
		Vec2f renderOffset;
		Vec2f size;

		Vec2f uv1;
		Vec2f uv2;
	};

	struct CharacterMetrics
	{		
		float advance;
		Vec2f renderOffset;
		Vec2f size;
	};
		               
	class BLAZE_API FontResolution
	{		
		struct CharacterUV
		{
			Vec2f uv1;
			Vec2f uv2;

			CharacterMetrics* metrics;
		};

		Font* font;
		uint resolution;
		Graphics::Core::Texture2D atlas;
		std::unordered_map<uint, CharacterUV> characterMap;
		
		void LoadAtlas();
	public:

		CharacterData GetCharacterData(UnicodeChar ch);
		uint GetResolution() const { return resolution; }		

		friend class Font;
	};

	class BLAZE_API Font
	{		
		void* ptr;					

		std::list<FontResolution> resolutions;				
		std::unordered_map<uint, CharacterMetrics> characterMap;		
		float baselineDistance;
		uint characterCount;
		uint pixelsPerUnit; //How many pixels fit in one font unit

		void LoadFontMetrics();
	public:
		Font();		
		~Font();				
					
		int Load(StringView path, bool emitLogOnFail = true);
		FontResolution* CreateFontResolution(uint resolution);				
				
		inline uint GetBaselineDistance() const { return baselineDistance; }

		friend class FontResolution;
	};	
}