#include "pch.h"
#include <BlazeEngine/UI/Text/TextStyle.h>

namespace Blaze::UI::Nodes
{
    TextStyle TextStyle::Scale(float scale) const
    {
        TextStyle out = *this;
        out.fontHeight *= scale;
        out.baselineOffset *= scale;
        return out;
    }
}