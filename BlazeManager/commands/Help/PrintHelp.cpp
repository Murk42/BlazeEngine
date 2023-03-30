#include "PrintHelp.h"
#include <iostream>

void PrintHelp()
{
	std::cout << "Available commands: \n";
	std::cout << 
		"	build, run, help, vsInfo, \n"
		"	setRuntimeLIB, setRuntimeDLL, setRuntimeProject, \n"
		"	setEngineLIB, setEngineDLL, setEngineProject, exit\n";	
}