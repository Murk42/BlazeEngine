#include "BlazeEngine/Console/Console.h"
#include <PDC/curses.h>

namespace Blaze
{
    namespace Console
    {

        void SetSize(const Vec2i& size)
        {
            resize_term(size.y, size.x);
        }
        char GetKeyPressed()
        {
            auto val = getch();
            refresh();
            return val;
        }

    }

    bool operator==(const char& c, const Console::Key& k)
    {
        return c == (char)k;
    }
    bool operator==(const Console::Key& k, const char& c)
    {
        return c == (char)k;
    }
}