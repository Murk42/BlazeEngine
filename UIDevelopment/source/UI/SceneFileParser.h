#pragma once
#include <stack>

class SceneFileParser
{
	enum class TokenType
	{
		Word,
		Literal,
		Special,
	};
	struct Token
	{
		TokenType type;
		String value;
		uint indent;
		uint line;
		uint position;
		uint index;
	};
	
public:
	struct Element
	{		
		uint indent;

		struct Property
		{
			String name;
			String value;
		};
		String typeName;
		std::vector<Property> properties;

		std::list<Element> children;
	};
private:
	struct FileData
	{
		struct Space
		{
			uint tokenIndex;
			String fullName;
			String parentName;
		};

		String fileData;

		std::vector<Token> tokens;

		std::stack<Space> spaces;
	};
	std::stack<FileData> files;
	FileData* currentFile;
	FileData::Space* currentSpace;
	Token* token;

	std::vector<std::list<Element>::iterator> elementBranch;

	bool ReadNextToken();
	bool ReadNextTokenAfter(uint index);
	
	Result Tokenize();

	Result Parse();
	Result ParseElement();
public:
	uint elementCount = 0;
	std::list<Element> elements;

	Result Load(Path path);
};