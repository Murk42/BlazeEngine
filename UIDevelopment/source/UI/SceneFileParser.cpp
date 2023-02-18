#include "SceneFileParser.h"

#include <ctype.h>

#define CHECK(x) { if (Result r = x) return r; }

#define IS_SPECIAL_VALUE(x) (token->value == x && token->type == TokenType::Special)
#define IS_WORD_VALUE(x) (token->value == x && token->type == TokenType::Word)

#define IS_SPECIAL() (token->type == TokenType::Special)
#define IS_WORD() (token->type == TokenType::Word)
#define IS_LITERAL() (token->type == TokenType::Literal)

#define TOKEN_INFO() ("ln: " + String::Convert(token->line) + ", ch: " + String::Convert(token->position))

bool SceneFileParser::ReadNextToken()
{
	if (currentSpace->tokenIndex + 1 < currentFile->tokens.size())
	{
		currentSpace->tokenIndex++;
		token = &currentFile->tokens[currentSpace->tokenIndex];
		return true;
	}
	return false;
}

bool SceneFileParser::ReadNextTokenAfter(uint index)
{
	while (token->index < index)
		if (!ReadNextToken())
			return false;

	return true;
}

Result SceneFileParser::Tokenize()
{
	enum class State {
		Undefined,
		None,
		Word,
		Special,
		Literal,
		Comment,
	};
	State state = State::Undefined;
	
	char prev = -1;

	uint indent = 0;
	uint line = 1;
	uint position = 0;
	bool newLine = true;

	uint start = 0;
	uint i = 0;
	while (i != currentFile->fileData.Size())
	{
		char ch = currentFile->fileData[i];

		if (ch == '\n')
		{			
			++line;
			indent = 0;
			newLine = true;
			position = 0;
		}
		else
			++position;
		
		bool parsed = false;
		while (!parsed)
		{
			switch (state)
			{
			case State::Undefined: {
				if (isalnum(ch))
					state = State::Word;
				else if (ch == '"')
				{
					state = State::Literal;
					parsed = true;
				}
				else if (ch == '#')
				{
					state = State::Comment;
					parsed = true;
				}
				else if (!isspace(ch))
					state = State::Special;
				else
					state = State::None;

				start = i;
				break;
			}
			case State::None: {
				if (newLine && ch == '\t')
					++indent;
				parsed = true;
				state = State::Undefined;
				break;
			}
			case State::Word: {	
				if (!isalnum(ch))
				{
					currentFile->tokens.push_back({ TokenType::Word, currentFile->fileData.SubString(start, i - start), indent, line, position + start - i, start });
					state = State::Undefined;
				}
				else
					parsed = true;
				break;
			}
			case State::Special: {
				currentFile->tokens.push_back({ TokenType::Special, String(&ch, 1), indent, line, position, i });
				state = State::Undefined;
				parsed = true;
				break;
			}
			case State::Literal: {
				if (ch == '"')
				{
					currentFile->tokens.push_back({ TokenType::Literal, currentFile->fileData.SubString(start + 1, i - start - 1), indent, line, position + start - i, start });
					state = State::Undefined;
				}
				parsed = true;
				break;
			}
			case State::Comment: {
				if (ch == '\n')
					state = State::Undefined;
				parsed = true;
				break;
			}
			}

			if (newLine && state != State::None && state != State::Undefined)
				newLine = false;
		}

		++i;
		prev = ch;
	}

	return Result();
}

Result SceneFileParser::Parse()
{
	currentSpace = &currentFile->spaces.emplace();
	token = &currentFile->tokens[0];

	while (true)
	{
		if (IS_SPECIAL_VALUE("<"))
			CHECK(ParseElement())
		else
			return BLAZE_ERROR_RESULT("Client", "Unexpected token. " + TOKEN_INFO());

		if (!ReadNextToken())
			break;
	}

	return Result();
}

Result SceneFileParser::ParseElement()
{
	uint indent = token->indent;
	while (!elementBranch.empty() && indent <= elementBranch.back()->indent)
		elementBranch.pop_back();

	ReadNextToken();

	if (!IS_WORD())
		return BLAZE_ERROR_RESULT("Client", "Expected a object type. " + TOKEN_INFO());

	Element el;	
	el.indent = indent;
	el.typeName = token->value;	

	ReadNextToken();

	while (true)
	{
		if (IS_SPECIAL_VALUE(">"))
			break;
		
		if (!IS_SPECIAL_VALUE(","))
			return BLAZE_ERROR_RESULT("Client", "Expected a ','. " + TOKEN_INFO());

		ReadNextToken();

		if (!IS_WORD())
			return BLAZE_ERROR_RESULT("Client", "Expected a word. " + TOKEN_INFO());

		auto& pr = el.properties.emplace_back();
		pr.name = token->value;

		ReadNextToken();

		if (!IS_SPECIAL_VALUE("="))
			return BLAZE_ERROR_RESULT("Client", "Expected a '='. " + TOKEN_INFO());

		ReadNextToken();

		if (IS_SPECIAL_VALUE(","))
			continue;

		int bracket1Depth = 0; //()
		int bracket2Depth = 0; //[]
		int bracket3Depth = 0; //{}
		int bracket4Depth = 0; //<>
		bool literal = false;

		String& data = currentFile->fileData;;
		size_t end = token->index;
		char ch = data[end];

		auto ChInfo = [&]() {
			return "ln: " + String::Convert(token->line) + ", ch: " + String::Convert(end - token->index + token->position);
		};
		auto UpdateBrackets = [&](char a, char b, int& count)
		{
			if (ch == a) ++count;
			if (ch == b)
			{
				if (count == 0) return BLAZE_ERROR_RESULT("Client", "Unexpected '" + String(&b, 1) + "'. " + ChInfo());
				--bracket1Depth;
			}

			return Result();
		};

		while (true)
		{
			if (!literal && bracket1Depth == 0 && bracket2Depth == 0 && bracket3Depth == 0 && bracket4Depth == 0 && 
				(ch == ',' || ch == '>'))
				break; 

			if (!literal)
			{
				CHECK(UpdateBrackets('(', ')', bracket1Depth));
				CHECK(UpdateBrackets('[', ']', bracket2Depth));
				CHECK(UpdateBrackets('{', '}', bracket3Depth));
				CHECK(UpdateBrackets('<', '>', bracket4Depth));
			}

			if (ch == '"')
				literal = !literal;

			if (ch == '\n')
				return BLAZE_ERROR_RESULT("Client", "Unexpected end of line. ln: " + String::Convert(token->line));

			++end;
			if (end == data.Size())
				return BLAZE_ERROR_RESULT("Client", "Unexpected end of file.");
			ch = data[end];
		}

		pr.value = data.SubString(token->index, end - token->index);

		ReadNextTokenAfter(end);
	}
	
	if (elementBranch.size() == 0)
	{
		elements.emplace_back(el);
		auto it = --elements.end();
		elementBranch.emplace_back(it);
	}
	else
	{
		elementBranch.back()->children.emplace_back(el);
		auto it = --elementBranch.back()->children.end();
		elementBranch.emplace_back(it);
	}

	++elementCount;

	return Result();
}

Result SceneFileParser::Load(Path path)
{
	File file(path, FileOpenMode::Read);
	currentFile = &files.emplace();
	currentFile->fileData.Resize(file.Size() + 1);
	file.Read(currentFile->fileData.Ptr(), currentFile->fileData.Size());
	file.Close();

	CHECK(Tokenize());
	CHECK(Parse());

	return Result();
}
