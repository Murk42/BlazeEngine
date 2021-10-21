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

                while (true)
                {
                    engine->Console.Input.m.lock();

                    if (engine->Console.Input.exit == true)
                    {
                        engine->Console.Input.m.unlock();
                        return;
                    }

                    if (engine->Console.Input.empty == true)
                        break;

                    engine->Console.Input.m.unlock();

                    std::this_thread::sleep_for(std::chrono::microseconds(100));
                }

                engine->Console.Input.empty = false;
                engine->Console.Input.data = std::move(String(s.data(), s.size()));
                engine->Console.Input.m.unlock();

            }
        }
        
        void Write(StringView text)
        {
            std::lock_guard<std::mutex> lk(engine->Console.coutMutex);
            std::cout << text.Ptr();
        }
        void Write(char text)
        {
            std::lock_guard<std::mutex> lk(engine->Console.coutMutex);
            std::cout << text;
        }
        void WriteLine(StringView text)
        {
            std::lock_guard<std::mutex> lk(engine->Console.coutMutex);
            std::cout << text.Ptr() << '\n';
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
        void ClearInput()
        {
            String s;
            ReadWord(s);
            std::cin.clear();
        }
        void BLAZE_API WaitForInput()
        {
            bool empty = true;

            while (empty)
            {
                engine->Console.Input.m.lock();
                empty = engine->Console.Input.empty;
                engine->Console.Input.m.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    }
}