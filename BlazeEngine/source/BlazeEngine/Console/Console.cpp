#include "pch.h"
#include "BlazeEngine/Console/Console.h"
#include "BlazeEngine/Internal/BlazeEngineContext.h"
#include <string>
#include <codecvt>
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
        template<>
        void Write<>(StringViewUTF8 text)
        {           
            consoleOutputStream.Write(text.Buffer(), text.BufferSize() - 1);            
        }                
        
        StringUTF8 Read()
        {
            std::wstring input;
            std::getline(std::wcin, input);
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            auto temp = converter.to_bytes(input);
            return StringUTF8((const void*)temp.data(), temp.size());
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

