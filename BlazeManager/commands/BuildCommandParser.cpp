#include <iostream>
#include <filesystem>
#include "BuildCommandParser.h"
#include "../Parsing.h"
#include "../Build.h"

#define CHECK(x) { if (x) return true; }

void PrintHelp()
{
	cout << "build <project path> <output dir> <output name> [-p:(property name)=(value)]\n";
}

static bool GetProjectPathFromArgs(const std::vector<string>& args, string& path)
{
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
static bool GetOutputDirFromArgs(const std::vector<string>& args, string& dir)
{
	if (!StripQuotes(args[2], dir))
	{
		cout << "Invalid output dir sysntax \"" << args[2] << "\"\n";
		return true;
	}

	std::error_code ec;

	if (!filesystem::exists(dir, ec))
	{
		if (ec)
		{
			cout << ec.category().name() << ": " << ec.message();
			return true;
		}

		filesystem::create_directories(dir, ec);

		if (ec)
		{
			cout << ec.category().name() << ": " << ec.message();
			return true;
		}
	}

	if (dir.back() != '\\')
		dir += '\\';

	return false;
}
static bool GetOutputNameFromArgs(const std::vector<string>& args, string& name)
{
	if (!StripQuotes(args[3], name))
	{
		cout << "Invalid output name \"" << args[3] << "\"\n";
		return true;
	}
	return false;
}

static bool ParseConfigurationProperty(const vector<string>& symbols, BuildCommandOptions& options)
{
	if (symbols.size() == 4)
	{
		cout << "Invalid configuration property syntax\n";
		return true;
	}

	if (symbols[4] == "=")
	{
		if (symbols.size() == 5)
		{
			cout << "Invalid configuration property syntax\n";
			return true;
		}

		if (symbols[5] == "Debug")
			options.configuration = Configuration::FinalBuild_Debug;
		else if (symbols[5] == "Release")
			options.configuration = Configuration::FinalBuild_Release;
		else
		{
			cout << "Invalid configuration value\n";
			return true;
		}
	}
	else
	{
		cout << "Invalid configuration property syntax\n";
		return true;
	}

	return false;
}
static bool ParsePlatformProperty(const vector<string>& symbols, BuildCommandOptions& options)
{
	if (symbols.size() == 4)
	{
		cout << "Invalid platform property syntax\n";
		return true;
	}

	if (symbols[4] == "=")
	{
		if (symbols.size() == 5)
		{
			cout << "Invalid platform property syntax\n";
			return true;
		}

		if (symbols[5] == "x86")
			options.platform = Platform::x86;
		else if (symbols[5] == "x64")
			options.platform = Platform::x64;
		else
		{
			cout << "Invalid platform property value\n";
			return true;
		}
	}
	else
	{
		cout << "Invalid platform property syntax\n";
		return true;
	}

	return false;
}

static bool ParsePropertySwitch(const vector<string>& symbols, BuildCommandOptions& options)
{
	if (symbols.size() == 3)
	{
		cout << "Invalid property switch syntax\n";
		return true;
	}

	if (symbols[3] == "Configuration")
	{
		CHECK(ParseConfigurationProperty(symbols, options));
	}
	else if (symbols[3] == "Platform")
	{
		CHECK(ParsePlatformProperty(symbols, options));
	}
	else
	{
		cout << "Invalid property name\n";
		return true;
	}

	return false;
}

static bool ParseSwitch(const vector<string>& symbols, BuildCommandOptions& options)
{
	if (symbols.size() >= 3 && symbols[1] == "p" && symbols[2] == ":")
	{
		CHECK(ParsePropertySwitch(symbols, options));
	}
	else
	{
		cout << "Invalid property switch syntax\n";
		return true;
	}

	return false;
}

static bool ParseSwitches(const vector<string>& args, BuildCommandOptions& options)
{
	size_t argOffset = 0;
	while (argOffset != args.size())
	{
		string arg = args[argOffset];
		auto symbols = SplitSymbols(arg);

		if (symbols[0] == "-")
			if (ParseSwitch(symbols, options))
			{
				cout << "Invalid argument \"" << arg << "\"\n";
				return true;
			}

		++argOffset;
	}

	return false;
}

bool ParseBuildCommand(const vector<string>& args, BuildCommandOptions& options)
{
	if (args.size() == 2 && args[1] == "help")
	{
		PrintHelp();
		return true;
	}
	else if (args.size() < 4)
	{
		cout << "Not enought arguments. Call \"build help\" for help\n";
		return true;
	}

	options.configuration = Configuration::FinalBuild_Debug;
	options.platform = Platform::x64;	
	
	CHECK(ParseSwitches(args, options));

	CHECK(GetProjectPathFromArgs(args, options.projectPath));			
	CHECK(GetOutputDirFromArgs(args, options.outputDir));
	CHECK(GetOutputNameFromArgs(args, options.outputName));		

	return false;
}