#include "pch.h"
#include "BlazeEngine/Console/Console.h"
#include "BlazeEngine/Internal/GlobalData.h"
#include <iostream>

namespace Blaze
{    
    TimingResult InitializeConsole()
    {        
        Timing timing{ "Console" };
        Debug::Logger::AddOutputToConsole();
        return timing.GetTimingResult();
    }
    void TerminateConsole()
    {   
        Debug::Logger::RemoveOutputFromConsole();
    }

    namespace Console
    {        
        void Write(StringView text)
        {            
            std::cout.write(text.Ptr(), text.Count());
        }
        void Write(char text)
        {   
            std::cout.write(&text, 1);
        }
        void WriteLine(StringView text)
        {            
            Write(text);
            Write('\n');
        }        
        
        String Read()
        {
            std::string input;
            std::cin >> input;
            return String(input.c_str(), input.size());
        }
    }
    namespace Debug::Logger
    {
        void AddOutputToConsole()
        {
            AddOutputStream(globalData->consoleOutputStream);
        }
        void RemoveOutputFromConsole()
        {
            RemoveOutputStream(globalData->consoleOutputStream);            
        }
    }
}

