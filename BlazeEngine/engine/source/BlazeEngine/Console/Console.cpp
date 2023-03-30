#include "BlazeEngine/Console/Console.h"
#include "BlazeEngine/Utilities/Thread.h"
#include <iostream>
#include "BlazeEngine/Core/Startup.h"

namespace Blaze
{    
    Startup::ConsoleInitInfo InitializeConsole()
    {        
        Startup::ConsoleInitInfo initInfo;
        initInfo.initTime = 0;
        return initInfo;
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