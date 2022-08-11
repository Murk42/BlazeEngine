#pragma once
#include "Parsing.h"

bool IsWordChar(char ch)
{
	return ch >= '0' && ch <= '9' || ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch == '.';
}

vector<string> Split(string s)
{
	vector<string> out;

	/*
	0 - not inside word
	1 - inside word
	2 - inside literal
	3 - literal end
	*/
	int state = 0;

	size_t begin = 0;
	size_t i = 0;
	while (i != s.size())
	{
		char ch = s[i];

		switch (state)
		{
		case 0:
			if (ch == '"')
			{
				begin = i;
				state = 2;
			}
			else if (!isspace(ch))
			{
				begin = i;
				state = 1;
			}
			break;
		case 1:
			if (isspace(ch))
			{
				out.emplace_back(move(s.substr(begin, i - begin)));
				state = 0;
			}
			break;
		case 2:
			if (ch == '"')
			{
				state = 3;
			}
			break;
		case 3:
			if (isspace(ch))
			{
				out.emplace_back(move(s.substr(begin, i - begin)));
				state = 0;
			}
			else
				state = 1;
			break;
		}
		++i;
	}

	if (state != 0)
		out.emplace_back(move(s.substr(begin, s.size() - begin)));

	return out;
}

vector<string> SplitSymbols(string s)
{
	vector<string> out;

	/*
	0 - outside word
	1 - inside word
	2 - inside literal
	*/
	int state = 0;

	size_t begin = 0;
	size_t i = 0;
	while (i != s.size())
	{
		char ch = s[i];

		switch (state)
		{
		case 0:
			if (ch == '"')
			{
				begin = i;
				state = 2;
			}
			else if (!IsWordChar(ch))
				out.emplace_back(move(s.substr(i, 1)));
			else
			{
				begin = i;
				state = 1;
			}
			break;
		case 1:
			if (isspace(ch))
			{
				out.emplace_back(move(s.substr(begin, i)));
				state = 0;
			}
			else if (!IsWordChar(ch))
			{
				out.emplace_back(move(s.substr(begin, i - begin)));
				out.emplace_back(move(s.substr(i, 1)));
				state = 0;
			}
			break;
		case 2:
			if (ch == '"')
			{
				state = 0;
				out.emplace_back(move(s.substr(begin, i)));
			}
			break;
		}
		++i;
	}

	if (state != 0)
		out.emplace_back(move(s.substr(begin, s.size() - begin)));

	return out;
}

bool StripQuotes(string i, string& o)
{
	if (i[0] == '"')
	{
		if (i.back() != '"')
			return false;

		o = i.substr(1, i.size() - 2);
	}
	else
		o = i;

	return true;
}

size_t GetWord(string in, string& out, int offset)
{
	/*
	0 - outside word
	1 - inside word
	*/
	int state = 0;

	int start = 0;
	int i = offset;
	while (i != in.size())
	{
		char ch = in[i];
		switch (state)
		{
		case 0:
			if (ch >= '0' && ch <= '9' || ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z')
			{
				start = i;
				state = 1;
			}
			break;
		case 1:
			if (!(ch >= '0' && ch <= '9' || ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z'))
			{
				out = move(in.substr(start, i - start));
				return i;
			}
			break;
		}
	}

	out = move(in.substr(start, i - start));
	return i;
}