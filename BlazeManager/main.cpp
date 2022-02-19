#include <iostream>

#include "VisualStudioInfo.h"
#include "Parsing.h"
#include "commands/BuildCommandParser.h"
#include "commands/RunCommandParser.h"

VisualStudioInfo vsInfo;
string blazeDir = "C:\\Programming\\Projects\\BlazeEngine\\BlazeEngine\\";
string runtimeDir = "C:\\Programming\\Projects\\BlazeEngine\\BlazeEngineRuntime\\";

string GetLine()
{	
	char ch;
	string o;
	while ((ch = cin.get()) != '\n')
	{		
		o += ch;
	}	
	
	return o;
}

int main(int argc, char* argv[])
{			
	vsInfo = GetVisualStudioInfo();

	while (true)
	{						
		vector<string> args;

		if (argc > 1)
		{
			for (int i = 1; i < argc; ++i)
				args.emplace_back(argv[i]);
			argc = 1;
		}
		else
		{
			cout << "->";
			string input = GetLine();
			args = Split(input);
		}

		if (args.size() == 0)
			continue;

		if (args[0] == "build")
		{
			BuildCommandOptions options;
			if (ParseBuildCommand(args, options)) continue;
			if (BuildCommand(options)) continue;
		}		
		else if (args[0] == "run")
		{
			RunCommandOptions options;
			if (ParseRunCommand(args, options)) continue;
			if (RunCommand(options)) continue;
		}
	}

	return 0;
}

//final "C:\Programming\Projects\BlazeEngineTest\Client\Client.vcxproj" "C:\Users\Marko\Desktop\temp\" App.exe -p:Configuration=Release -p:Platform=x86