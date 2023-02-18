#include <iostream>
#include <filesystem>

#include "Globals.h"

#include "Result.h"
#include "Parsing.h"
#include "commands/Build/BuildCommandParser.h"
#include "commands/Run/RunCommandParser.h"
#include "commands/Help/PrintHelp.h"

string OpenFileGUI();

string GetLine()
{	
	char ch;
	string o;
	while ((ch = cin.get()) != '\n')	
		o += ch;	
	
	return o;
}

bool superDebug = false;

vector<string> EvaluateProgramArguments(int& argc, char* argv[])
{
	int startArgument = 1;

	if (argc > 1 && strcmp(argv[1], "superDebug") == 0)
	{
		superDebug = true;
		startArgument = 2;
	}

	if (superDebug)
	{
		cout << "superDebug flag set\n";
		cout << "BlazeManager: starting program\n";
		cout << "BlazeManager: argc = " << argc << " argv = { ";

		for (int i = 0; i < argc; ++i)
			cout << argv[i] << (i == argc - 1 ? " }\n" : ", ");
	}

	if (!std::filesystem::exists(engineProjectDir)) engineProjectDir.clear();
	if (!std::filesystem::exists(runtimeProjectDir)) engineProjectDir.clear();

	GetVisualStudioInfo(vsInfo);

	if (superDebug)
	{
		cout << "\n";
		cout << "BlazeManager: vsInfo: \n";
		cout << "BlazeManager: devenvPath = " << vsInfo.devenvPath << "\n";
		cout << "BlazeManager: MSBuildPath = " << vsInfo.MSBuildPath << "\n";
		cout << "BlazeManager: instalationPath = " << vsInfo.instalationPath << "\n";
		cout << "\n";
	}

	vector<string> args;

	if (argc > startArgument)
	{
		if (superDebug)
			cout << "BlazeManager: Parsing command line arguments\n";
		for (int i = startArgument; i < argc; ++i)
			args.emplace_back(argv[i]);
		argc = 1;
	}	

	return args;
}

void ReadCommandFromConsole(vector<string>& args)
{
	if (args.empty())
	{
		if (superDebug)
			cout << "BlazeManager: Waiting for input\n";

		cout << "->";
		string input = GetLine();

		if (superDebug)
			cout << "BlazeManager: Parsing input...";

		args = Split(input);
	}
}

bool ExecuteCommand(vector<string>& args)
{
	if (args.size() == 0)
		return true;

	if (args[0] == "build")
	{
		if (superDebug)
			cout << "BlazeManager: Starting build command";

		BuildCommandOptions options;
		if (ParseBuildCommand(args, options)) return true;
		if (BuildCommand(options)) return true;
	}
	else if (args[0] == "run")
	{
		if (superDebug)
			cout << "BlazeManager: Starting run command";

		RunCommandOptions options;
		if (Result r = ParseRunCommand(args, options)) return true;
		if (Result r = RunCommand(options))
		{
			if (options.exitCondition == RunCommandExitCondition::ForceExit)
				return false;
			else
				return true;
		}

		if (options.exitCondition == RunCommandExitCondition::ForceExit)
			return false;
	}
	else if (args[0] == "setRuntimeLIB")
	{
		runtimeLIBPath = OpenFileGUI();
	}
	else if (args[0] == "setRuntimeDLL")
	{
		runtimeDLLPath = OpenFileGUI();
	}
	else if (args[0] == "setRuntimeProject")
	{
		runtimeProjectDir = OpenFileGUI();
	}
	else if (args[0] == "setEngineLIB")
	{
		engineLIBPath = OpenFileGUI();
	}
	else if (args[0] == "setEngineDLL")
	{
		engineDLLPath = OpenFileGUI();
	}
	else if (args[0] == "setEngineProject")
	{
		engineProjectDir = OpenFileGUI();
	}
	else if (args[0] == "help")
	{
		PrintHelp();
	}
	else if (args[0] == "vsInfo")
	{
		PrintVisualStudioInfo(vsInfo);
	}
	else if (args[0] == "exit")
	{
		return false;
	}
	else
	{
		cout << "Invalid command. Type help for help\n";
	}

	return true;
}

int main(int argc, char* argv[])
{				
	vector<string> args = EvaluateProgramArguments(argc, argv);

	while (true)
	{			
		ReadCommandFromConsole(args);

		if (ExecuteCommand(args) == false)
			return 0;

		args.clear();
	}
}