#include "Result.h"

Result::Result()
	: sucessfull(true)
{

}
Result::Result(Result&& r) noexcept
	: sucessfull(r.sucessfull), log(std::move(r.log))
{

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

Result operator+(const std::string& s, const Result& r)
{
	return Result(s + r.log, r.sucessfull);
}
Result operator+(const Result& r, const std::string& s)
{
	return Result(r.log + s, r.sucessfull);
}