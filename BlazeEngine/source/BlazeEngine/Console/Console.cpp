#include "pch.h"
#include "BlazeEngine/Console/Console.h"
#include "BlazeEngine/Threading/Thread.h"
#include <iostream>

namespace Blaze
{    
    TimingResult InitializeConsole()
    {        
        Timing timing{ "Console" };
        return timing.GetTimingResult();
    }
    void TerminateConsole()
    {                
    }

    namespace Console
    {        
        void Write(StringView text)
        {            
            std::cout.write(text.Ptr(), text.Size());
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
}