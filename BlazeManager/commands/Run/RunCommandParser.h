#pragma once
#include <string>
#include <vector>
using namespace std;

#include "RunCommand.h"
#include "Result.h"

Result ParseRunCommand(const vector<string>& args, RunCommandOptions& options);