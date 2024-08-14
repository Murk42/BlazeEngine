#pragma once
#include "BlazeEngineGraphics/Text/AtlasData.h"

namespace Blaze::Graphics
{
	class FontAtlasesData
	{
	public:
		FontAtlasesData(Font* font);

		AtlasData* GetAtlasData(uint fontHeight);

		void AddAtlasData(const Array<uint>& fontHeights, const CharacterSet& characterSet);

		static FontAtlasesData& AddToFont(Font& font, const Array<uint>& fontHeights, const CharacterSet& characterSet);
		static FontAtlasesData* RetrieveFromFont(Font& font);
	private:
		List<AtlasData> atlasData;
		Font* font;
	};
}