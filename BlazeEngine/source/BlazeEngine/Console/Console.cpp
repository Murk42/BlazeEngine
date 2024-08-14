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
        void Write(StringViewUTF8 text)
        {           
            consoleOutputStream.Write(text.Buffer(), text.BufferSize() - 1);            
        }
        void Write(char text)
        {   
            consoleOutputStream.Write(&text, 1);
        }
        void WriteLine(StringViewUTF8 text)
        {            
            Write(text);
            Write('\n');
        }        
        
        StringUTF8 Read()
        {
            std::string input;
            std::cin >> input;
            return StringUTF8((const void*)input.c_str(), input.size());
        }
    }
    namespace Debug::Logger
    {
        void AddOutputToConsole()
        {
            AddOutputStream(consoleOutputStream);
        }
        void RemoveOutputFromConsole()
        {
            RemoveOutputStream(consoleOutputStream);            
        }
    }
}

