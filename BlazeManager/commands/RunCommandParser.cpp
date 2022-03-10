#include "RunCommandParser.h"
#include "../Parsing.h"
#include <iostream>
#include <filesystem>

#define CHECK(x) { if (x) return true; }

static void PrintHelp()
{
	cout << "\n ---- run help ----\n";
	cout << "run <project path> [switches]\n\n";
	cout << "A switch can be one of \"Release\", \"exit\", \"dontBuildBlaze\", \"dontBuildClient\", \"dontBuildRuntime\", \"dontBuildAny\", \"noLog\".\n";
	cout << "\n";
}

static bool GetProjectPathFromArgs(const std::vector<string>& args, string& path)
{
	if (args[1] == "\"\"")
		return false;

	if (!StripQuotes(args[1], path))
	{
		cout << "Invalid project path syntax \"" << args[1] << "\"\n";
		return true;
	}

	std::error_code ec;
	if (!filesystem::exists(path, ec))
	{
		cout << "Invalid project path. The path does not exist \"" << args[1] << "\"\n";

		if (ec)	cout << ec.category().name() << ": " << ec.message();

		return true;
	}

	if (!filesystem::is_regular_file(path, ec))
	{
		cout << "Invalid project path. The path is not refering to a file \"" << args[1] << "\"\n";

		if (ec)	cout << ec.category().name() << ": " << ec.message();

		return true;
	}

	return false;
}

static bool ParseSwitch(const vector<string>& symbols, RunCommandOptions& options)
{		
	if (symbols.size() == 2)
	{
		if (symbols[1] == "Release")
		{
			options.release = true;
		}
		else if (symbols[1] == "exit")
		{
			options.stopAfter = true;
		}
		else if (symbols[1] == "dontBuildBlaze")
		{
			options.dontBuildBlaze = true;
		}
		else if (symbols[1] == "dontBuildClient")
		{
			options.dontBuildClient = true;
		}
		else if (symbols[1] == "dontBuildRuntime")
		{
			options.dontBuildRuntime = true;
		}
		else if (symbols[1] == "dontBuildAny")
		{
			options.dontBuildBlaze = options.dontBuildClient = options.dontBuildRuntime = true;
		}
		else if (symbols[1] == "noLog")
		{
			options.log = false;
		}
		else if (symbols[1] == "guiOpenFile")
		{
			options.guiOpenFile = true;
		}
	}
	else
	{
		cout << "Invalid property switch syntax\n";
		return true;
	}

	return false;
}

static bool ParseSwitches(const vector<string>& args, RunCommandOptions& options)
{
	size_t argOffset = 0;
	while (argOffset != args.size())
	{
		string arg = args[argOffset];
		auto symbols = SplitSymbols(arg);

		if (symbols[0] == "-")
			CHECK(ParseSwitch(symbols, options));

		++argOffset;
	}

	return false;
}

bool ParseRunCommand(const vector<string>& args, RunCommandOptions& options)
{
	if (args.size() == 2 && args[1] == "help")
	{
		PrintHelp();
		return true;
	}
	else if (args.size() < 2)
	{
		cout << "Not enought arguments. Call \"run help\" for help\n";
		return true;
	}

	options.log = true;
	options.release = false;
	options.stopAfter = false;
	options.dontBuildBlaze = false;
	options.dontBuildClient = false;
	options.dontBuildRuntime = false;
	options.guiOpenFile = false;

	CHECK(ParseSwitches(args, options));

	CHECK(GetProjectPathFromArgs(args, options.projectPath));		

	return false;
}