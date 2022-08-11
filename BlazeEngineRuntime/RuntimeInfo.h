#pragma once

struct RuntimeInfo
{
	bool runtimeLog;
	struct {
		double other = 0;
		double loadingBlazeLibraries = 0;
		double loadingBlaze = 0;
		double loadingRuntime = 0;
		double loadingClientLibraries = 0;
		double loadingClient = 0;
		double all = 0;
	} timings;
};