#pragma once

namespace Blaze
{
	void ExecuteOnMainThread(void(*function)(void*), void* userData);
}