#include "BlazeEngine/Console/Console.h"
#include "source/BlazeEngine/Internal/Engine.h"
#include <PDC/curses.h>
#include <iostream>
#include <string>

namespace Blaze
{
    namespace Console
    {        
        void ConsoleInputThreadFunction()
        {                
            while (true)
            {
                std::string s;
                std::cin >> s;

                engine->Console.Input.m.lock();
                engine->Console.Input.empty = false;
                engine->Console.Input.data = std::move(String(s.data(), s.size()));
                engine->Console.Input.m.unlock();
            }
        }
        
        void Write(const StringView& text)
        {
            std::cout << text.Ptr();
        }
        bool ReadWord(String& value)
        {
            String copy;
            bool empty;

            engine->Console.Input.m.lock();
            copy = std::move(engine->Console.Input.data);
            empty = engine->Console.Input.empty;
            engine->Console.Input.empty = true;
            engine->Console.Input.m.unlock();
            
            if (!empty)
            {
                value = std::move(copy);
                return true;
            }            
            return false;
        }
    }
}