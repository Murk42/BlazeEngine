#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Core/Logger.h"
#include <system_error>

namespace Blaze
{
	int ResolveError(std::error_code& ec)
	{
		if (ec)
		{
			BLAZE_ERROR_LOG(ec.category().name(), ec.message().c_str());
			return BLAZE_ERROR;
		}
		return BLAZE_OK;
	}
	int ResolveError(int value, std::error_code& ec)
	{
		if (ec)
		{
			BLAZE_ERROR_LOG(ec.category().name(), ec.message().c_str());
			return BLAZE_ERROR;
		}
		return value;
	}
}