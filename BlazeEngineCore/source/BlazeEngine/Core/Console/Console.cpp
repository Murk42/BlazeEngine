#include "pch.h"
#include "BlazeEngine/Core/Console/Console.h"
#include "BlazeEngine/Core/BlazeEngineCoreContext.h"

namespace Blaze
{
    namespace Console
    {
        template<>
        void Write<>(u8StringView text)
        {
            blazeEngineCoreContext.consoleOutputStream.Write(text.Ptr(), text.Count());
        }

        u8String Read()
        {
            return { };
        }
    }
}

