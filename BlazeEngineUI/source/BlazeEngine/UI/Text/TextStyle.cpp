#include "pch.h"
#include <BlazeEngine/UI/Text/TextStyle.h>

namespace Blaze::UI::Nodes
{
    TextStyle TextStyle::Scale(float scale) const
    {
        TextStyle out = *this;
        out.fontHeight = static_cast<uint32>(fontHeight * scale);
        out.baselineOffset *= scale;
        return out;
    }
    FontManager::FontData TextStyle::GetFontData() const
    {
        if (fontManager == nullptr)
            return { };

        FontManager::FontData fontData;
        if (!fontManager->GetFontAtlas(fontName, fontHeight, fontData))        
            return { };

        return fontData;
    }
}