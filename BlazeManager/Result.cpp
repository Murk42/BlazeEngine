#include "Result.h"
#include <iostream>

Result::Result()
	: sucessfull(true)
{

}
Result::Result(Result&& r) noexcept
	: sucessfull(r.sucessfull), log(std::move(r.log))
{
	r.sucessfull = true;
}
Result::Result(std::string l)
	: log(l), sucessfull(false)
{

}

Result::Result(std::string l, bool s)
	: log(l), sucessfull(s)
{

}
Result::~Result()
{
	if (!log.empty())
		std::cout << log << std::endl;
}


Result::operator bool() const
{
	return !sucessfull;
}

Result& Result::operator=(const Result& r)
{
	log = r.log;
	sucessfull = r.sucessfull;

	return *this;
}

Result operator+(const std::string& s, Result&& r)
{	
	r.log = s + r.log;
	return std::move(r);
}
Result operator+(Result&& r, const std::string& s)
{
	r.log = r.log + s;
	return std::move(r);
}