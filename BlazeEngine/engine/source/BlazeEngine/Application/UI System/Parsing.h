#pragma once
#include <vector>
#include <string>
using namespace std;

bool IsWordChar(char ch);

vector<string> Split(string s);

vector<string> SplitSymbols(string s);

bool StripQuotes(string i, string& o);

size_t GetWord(string in, string& out, int offset = 0);