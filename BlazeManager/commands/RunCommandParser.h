#pragma once
#include <string>
#include <vector>
using namespace std;

#include "RunCommand.h"

bool ParseRunCommand(const vector<string>& args, RunCommandOptions& options);