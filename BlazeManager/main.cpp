#include <iostream>
#include "VisualStudioInfo.h"
#include "Parsing.h"
#include "commands/Build/BuildCommandParser.h"
#include "commands/Run/RunCommandParser.h"
#include "commands/Help/PrintHelp.h"

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

bool superDebug = false;

int main(int argc, char* argv[])
{			
	if (argc > 1 && strcmp(argv[1], "superDebug") == 0)
		superDebug = true;

	if (superDebug)
	{
		cout << "superDebug flag set\n";
		cout << "BlazeManager: starting program\n";
		cout << "BlazeManager: argc = " << argc << " argv = { ";

		for (int i = 0; i < argc; ++i)
			cout << argv[i] << (i == argc - 1 ? " }\n" : ", ");
	}

	vsInfo = GetVisualStudioInfo();

	if (superDebug)
	{
		cout << "\n";
		cout << "BlazeManager: vsInfo: \n";
		cout << "BlazeManager: devenvPath = " << vsInfo.devenvPath << "\n";
		cout << "BlazeManager: MSBuildPath = " << vsInfo.MSBuildPath << "\n";
		cout << "BlazeManager: instalationPath = " << vsInfo.instalationPath << "\n";
		cout << "\n";
	}

	int startArgLowest = 1;
	if (superDebug)
		startArgLowest = 2;

	while (true)
	{						
		vector<string> args;

		if (argc > startArgLowest)
		{
			if (superDebug)
				cout << "BlazeManager: Parsing command line arguments\n";
			for (int i = startArgLowest; i < argc; ++i)
				args.emplace_back(argv[i]);
			argc = 1;
		}
		else
		{
			if (superDebug)
				cout << "BlazeManager: Waiting for input\n";

			cout << "->";
			string input = GetLine();

			if (superDebug)
				cout << "BlazeManager: Parsing input...";

			args = Split(input);
		}

		if (args.size() == 0)
			continue;

		if (args[0] == "build")
		{
			if (superDebug)
				cout << "BlazeManager: Starting build command";

			BuildCommandOptions options;
			if (ParseBuildCommand(args, options)) continue;
			if (BuildCommand(options)) continue;
		}		
		else if (args[0] == "run")
		{
			if (superDebug)
				cout << "BlazeManager: Starting run command";

			RunCommandOptions options;
			if (ParseRunCommand(args, options)) continue;
			bool ret = RunCommand(options);
			if (options.exitCondition == RunCommandExitCondition::ForceExit)
				return 0;
			if (ret)
				continue;
		}
		else if (args[0] == "help")
		{
			PrintHelp();
		}
		else if (args[0] == "vsInfo")
		{
			PrintVisualStudioInfo(vsInfo);
		}
	}
	
	return 0;
}

//build "C:\Programming\Projects\TemplateProjectlol1\TemplateProjectlol1\TemplateProjectlol1.vcxproj" "C:\Users\Marko\Desktop\temp" "program"
//final "C:\Programming\Projects\BlazeEngineTest\Client\Client.vcxproj" "C:\Users\Marko\Desktop\temp\" App.exe -p:Configuration=Release -p:Platform=x86