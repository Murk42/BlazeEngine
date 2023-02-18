#pragma once
#include <string>

struct Result
{
	std::string log;
	bool sucessfull;

	Result();
	Result(Result&&) noexcept;
	Result(std::string l);
	Result(std::string l, bool s);
	~Result();

	operator bool() const;

	Result& operator=(const Result& r);
};

Result operator+(const std::string& s, const Result& r);
Result operator+(const Result& r, const std::string& s);