#pragma once
#include <string>

struct Result
{	
	std::string log;
	bool sucessfull;

	Result()
		: sucessfull(true)
	{

	}
	Result(std::string l)
		: log(l), sucessfull(false)
	{

	}

	Result(std::string l, bool s)
		: log(l), sucessfull(s)
	{

	}
};