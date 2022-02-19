#pragma once
#include <string>
#include <vector>
using namespace std;

#include "BuildCommand.h"

bool ParseBuildCommand(const vector<string>& args, BuildCommandOptions& options);