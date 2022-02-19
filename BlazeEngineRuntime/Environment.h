#include <string>
using namespace std;

#define Literal(x) ("\"" x "\"")

struct EnvironmentSettings
{
	enum class Architecture
	{
		x86,		 //Host: x86, x64 Build: x86
		x86_amd64,	 //Host: x86, x64 Build: x64
		x86_arm,	 //Host: x86, x64 Build: ARM
		x86_arm64,	 //Host: x86, x64 Build: ARM64
		amd64,		 //Host: x64	   Build: x64
		amd64_x86,	 //Host: x64	   Build: x86
		amd64_arm,	 //Host: x64	   Build: ARM
		amd64_arm64, //Host: x64	   Build: ARM64		
	};
	enum class PlatformType
	{
		Store,
		UWP,
	};

	Architecture architecture;
	PlatformType platform_type;
	string winSDK_version;
	string vc_version;
};

const char* environmentArchitectureStrings[]{
	"x86",
	"x86_amd64",
	"x86_arm",
	"x86_arm64",
	"amd64",
	"amd64_x86",
	"amd64_arm",
	"amd64_arm64"
};
const char* environmentPlatformTypeStrings[]{
	"store",
	"uwp"
};

string SetupEnvironment(EnvironmentSettings settings)
{
	string architecture = environmentArchitectureStrings[(int)settings.architecture];
	string platform_type = environmentPlatformTypeStrings[(int)settings.platform_type];
	string batPath = "\"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat\"";
	string command = batPath + " " + architecture + " " + platform_type + " " + settings.winSDK_version + " " + settings.vc_version;
	return command;
}
