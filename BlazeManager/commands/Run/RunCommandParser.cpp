#include "RunCommandParser.h"
#include "Parsing.h"
#include <iostream>
#include <filesystem>

#include "OpenFileGUI.h"

static void PrintHelp()
{
	cout << "\n ---- run help ----\n";
	cout << "run <project path> [switches]\n\n";
	cout << "Available switches:\n"
		"\"Debug\" \"Release\", \"noExit\", \"forceExit\", \"buildBlaze\", \"buildClient\",\n"
		"\"buildRuntime\", \"buildAll\", \"noLog\", \"noManagerLog\", \"noRuntimeLog\", \n"
		"\"logTimings\" \"guiOpenFile\".\n";
	cout << "\n";
}

static Result EvaluateProjectPath(string arg, string& path)
{
	if (arg == "\"\"")
	{
#ifdef _WIN32
		path = OpenFileGUI();
		return Result();
#else
		return Result("Empty project path passed");
#endif			
	}

	if (!StripQuotes(arg, path))
		return Result("Invalid project path syntax \"" + arg + "\"\n");	

	std::error_code ec;

	if (!filesystem::exists(path, ec))
		return Result("Invalid project path. The path does not exist \"" + arg + "\"\n" + ec.category().name() + ": " + ec.message() + "\n");

	if (!filesystem::is_regular_file(path, ec))
		return Result("Invalid project path. The path is not refering to a file \"" + arg + "\"\n" + ec.category().name() + ": " + ec.message() + "\n");	

	return Result();
}

static Result ParseSwitch(const string& arg, RunCommandOptions& options)
{		
	auto symbols = SplitSymbols(arg);

	if (symbols[0] != "-")
		return Result("Invalid property switch syntax: \"" + arg + "\"\nSwitches have to start with a dash ('-')");

	if (symbols.size() == 2)
	{	
		if (symbols[1] == "Debug")
		{
			options.configuration = RuntimeConfiguration::Debug;
		}
		else if (symbols[1] == "Release")
		{			
			options.configuration = RuntimeConfiguration::Release;
		}
		else if (symbols[1] == "noExit")
		{
			options.exitCondition = RunCommandExitCondition::NoExit;
		}
		else if (symbols[1] == "forceExit")
		{
			options.exitCondition = RunCommandExitCondition::ForceExit;
		}		
		else if (symbols[1] == "buildBlaze")
		{
			options.buildEngine = true;
		}
		else if (symbols[1] == "buildClient")
		{
			options.buildClient = true;
		}
		else if (symbols[1] == "buildRuntime")
		{
			options.buildRuntime = true;
		}
		else if (symbols[1] == "buildAll")
		{
			options.buildEngine = options.buildClient = options.buildRuntime = true;
		}
		else if (symbols[1] == "noLog")
		{
			options.managerLog = false;
			options.runtimeLog = false;
		}
		else if (symbols[1] == "noManagerLog")
		{
			options.managerLog = false;
		}
		else if (symbols[1] == "noRuntimeLog")
		{
			options.runtimeLog = false;
		}		
		else if (symbols[1] == "logTimings")
		{
			options.logTimings = true;
		}		
		else		
			return Result("Unrecognized switch: \"" + symbols[1] + "\"\n", true);		
	}
	else	
		return Result("Invalid property switch syntax: \"" + arg + "\"");

	return Result();
}

static Result ParseSwitches(const vector<string>& args, RunCommandOptions& options, size_t argStartingIndex)
{
	size_t argOffset = argStartingIndex;
	while (argOffset != args.size())
	{
		string arg = args[argOffset];		
		
		if (Result r = ParseSwitch(arg, options))
			return r;

		++argOffset;
	}

	return Result();
}

Result ParseRunCommand(const vector<string>& args, RunCommandOptions& options)
{
	if (args.size() == 2 && args[1] == "help")
	{
		PrintHelp();
		return Result();
	}
	else if (args.size() < 2)
	{
		cout << "Not enought arguments. Call \"run help\" for help\n";
		return Result();
	}

	options.buildEngine = false;
	options.buildClient = false;
	options.buildRuntime = false;
	options.configuration = RuntimeConfiguration::Debug;
	options.exitCondition = RunCommandExitCondition::NoExit;
	options.logTimings = true;
	options.managerLog = true;
	options.projectPath = "";
	options.runtimeLog = true;

	if (Result r = EvaluateProjectPath(args[1], options.projectPath))
		return r;

	if (Result r = ParseSwitches(args, options, 2))
		return r;

	return Result();
}