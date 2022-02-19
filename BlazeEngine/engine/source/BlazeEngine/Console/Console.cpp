#include "BlazeEngine/Console/Console.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/Utilities/Thread.h"
#include <iostream>

namespace Blaze
{    
    void InitializeConsole()
    {        
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