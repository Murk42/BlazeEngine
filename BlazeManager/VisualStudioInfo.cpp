#include "VisualStudioInfo.h"
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>

#include "Command.h"

VisualStudioInfo GetVisualStudioInfo()
{
	VisualStudioInfo info;
	//info.	
	Result result = RunCommand("\"C:\\Program Files (x86)\\Microsoft Visual Studio\\Installer\\vswhere.exe\" -property installationPath");
	if (!result.sucessfull)
	{
		std::cout << result.log;
		std::cout << "Finding Visual Studio instalation path failed\n";
	}
	else
		info.instalationPath = result.log;
	info.instalationPath.pop_back();

	result = RunCommand("\"C:\\Program Files (x86)\\Microsoft Visual Studio\\Installer\\vswhere.exe\" -property productPath");
	if (!result.sucessfull)
	{
		std::cout << result.log;
		std::cout << "Finding Visual Studio devenv path failed";
	}
	else
		info.devenvPath = result.log;

	info.devenvPath.pop_back();

	info.MSBuildPath = info.instalationPath + "\\MSBuild\\Current\\Bin\\MSBuild.exe";

	return info;
}

void PrintVisualStudioInfo(VisualStudioInfo info)
{
	std::cout << "\n\tVisual Studio info\n";
	std::cout << "instalationPath = \"" << info.instalationPath << "\"\n";
	std::cout << "devenvPath      = \"" << info.devenvPath << "\"\n";
	std::cout << "MSBuildPath     = \"" << info.MSBuildPath << "\"\n";
	std::cout << "\n";

}

//"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" - property catalog_productDisplayVersion

/*
instanceId: 3d65b012
installDate: 23.5.2019. 18:06:30
installationName: VisualStudio/16.11.7+31911.196
installationPath: C:\Program Files (x86)\Microsoft Visual Studio\2019\Community
installationVersion: 16.11.31911.196
productId: Microsoft.VisualStudio.Product.Community
productPath: C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\devenv.exe
state: 4294967295
isComplete: 1
isLaunchable: 1
isPrerelease: 0
isRebootRequired: 0
displayName: Visual Studio Community 2019
description: Powerful IDE, free for students, open-source contributors, and individuals
channelId: VisualStudio.16.Release
channelUri: https://aka.ms/vs/16/release/channel
enginePath: C:\Program Files (x86)\Microsoft Visual Studio\Installer\resources\app\ServiceHub\Services\Microsoft.VisualStudio.Setup.Service
releaseNotes: https://docs.microsoft.com/en-us/visualstudio/releases/2019/release-notes-v16.11#16.11.7
thirdPartyNotices: https://go.microsoft.com/fwlink/?LinkId=660909
updateDate: 2021-11-28T13:45:19.3066606Z
catalog_buildBranch: d16.11
catalog_buildVersion: 16.11.31911.196
catalog_id: VisualStudio/16.11.7+31911.196
catalog_localBuild: build-lab
catalog_manifestName: VisualStudio
catalog_manifestType: installer
catalog_productDisplayVersion: 16.11.7
catalog_productLine: Dev16
catalog_productLineVersion: 2019
catalog_productMilestone: RTW
catalog_productMilestoneIsPreRelease: False
catalog_productName: Visual Studio
catalog_productPatchVersion: 7
catalog_productPreReleaseMilestoneSuffix: 1.0
catalog_productSemanticVersion: 16.11.7+31911.196
catalog_requiredEngineVersion: 2.11.52.58712
properties_campaignId: Unity3d_Unity
properties_canceled: 0
properties_channelManifestId: VisualStudio.16.Release/16.11.7+31911.196
properties_nickname:
properties_setupEngineFilePath: C:\Program Files (x86)\Microsoft Visual Studio\Installer\vs_installershell.exe

C:\Users\Marko>
*/