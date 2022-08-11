#include "UISceneParser.h"
#include "BlazeEngine/File/File.h"
#include "Parsing.h"

#include "BlazeEngine/Application/UI System/Core Elements/Button.h"
#include "BlazeEngine/Application/UI System/Core Elements/Image.h"
#include "BlazeEngine/Application/UI System/Core Elements/Panel.h"
#include "BlazeEngine/Application/UI System/Core Elements/Text.h"
#include "BlazeEngine/Application/UI System/Core Elements/TexturedPanel.h"
#include "BlazeEngine/File/FileSystem.h"

#define EXPECT(msg) if (GetNextToken()) return BadResult(msg);

#define EXPECT_VALUE(x, noTokenMsg, invalidValueMsg) if (GetNextToken()) return BadResult(noTokenMsg); if (token.value != x) return BadResult(invalidValueMsg);
#define EXPECT_TYPE(x, noTokenMsg, invalidTypeMsg) if (GetNextToken()) return BadResult(noTokenMsg); if (token.type != x) return BadResult(invalidTypeMsg);

#define EXPECT_WORD(noTokenMsg, invalidValueMsg)	if (GetNextToken()) return BadResult(noTokenMsg); if (token.type != Word)																return BadResult(invalidValueMsg);
#define EXPECT_LITERAL(noTokenMsg, invalidValueMsg)	if (GetNextToken()) return BadResult(noTokenMsg); if (token.type != Literal)															return BadResult(invalidValueMsg);
#define EXPECT_NUMBER(noTokenMsg, invalidValueMsg)	if (GetNextToken()) return BadResult(noTokenMsg); if (token.type != NumberFloat && token.type != NumberInt && token.type != NumberHex)	return BadResult(invalidValueMsg);
#define EXPECT_INT(noTokenMsg, invalidValueMsg)		if (GetNextToken()) return BadResult(noTokenMsg); if (token.type != NumberInt && token.type != NumberHex)								return BadResult(invalidValueMsg);

#define CHECK_NEXT_TOKEN()	(GetNextToken())

#define CHECK_LITERAL(msg) if (token.type != Literal) return BadResult(msg);
#define CHECK_VALUE(x, msg) if (token.value != x) return BadResult(msg);
#define CHECK_TYPE(x, msg) if (token.type != x) return BadResult(msg);

#define CONTINUE_IF_NEXT_VALUE(x) if (GetNextToken()) return Result(); if (token.value != x) { ReverseToken(); return Result(); }

#define COMPARE_VALUE(x) (token.value == x)
#define COMPARE_TYPE(x) (token.type == x)

namespace Blaze::UI
{
	Result BadResult()
	{
		return Result(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, "BlazeEngine",
			"Error!"));
	}
	Result BadResult(std::string msg)
	{		
		return Result(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, "BlazeEngine",
			String(msg.data(), msg.size())));
	}

	bool UISceneParser::GetNextToken()
	{
		return GetNextToken("");
	}
	bool UISceneParser::GetNextToken(std::string msg)
	{
		if (tokenIndex < tokens.size())
		{
			token = tokens[tokenIndex++];
			return false;
		}
		else
		{
			Logger::AddLog(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, "BlazeEngine", msg.c_str());
			return true;
		}
	}
	void UISceneParser::ReverseToken()
	{
		tokenIndex--;
	}
	Result UISceneParser::LoadFile(Path path)
	{
		File file{ path, FileOpenMode::Read };
		Buffer buffer{ file.Size() };
		file.Read(buffer);
		fileText.resize(buffer.Size());
		memcpy(fileText.data(), buffer.Ptr(), buffer.Size());

		return Result();
	}
	Result UISceneParser::RemoveComments()
	{		
		uint state = 0;
		uint i = 0;
		uint begin;
		char prev = ' ';
		while (i != fileText.size())
		{
			char ch = fileText[i];
			switch (state)
			{
			case 0:
				if (prev == '/' && ch == '/')
				{
					begin = i - 1;
					state = 1;
				}
				if (prev == '/' && ch == '*')
				{
					begin = i - 1;
					state = 2;
				}
				break;
			case 1:
				if (ch == '\n')
				{
					fileText.erase(begin, i - begin + 1);
					i = begin;
					state = 1;
				}
				break;
			case 2:
				if (prev == '*' && ch == '/')
				{
					fileText.erase(begin, i - begin + 1);
					i = begin;
					state = 0;
				}
				break;
			}

			prev = ch;
			++i;
		}		

		return Result();
	}
	Result UISceneParser::Tokenize()
	{
		uint state = 0;
		/*
		0 - nothing
		1 - word
		2 - number
		3 - special symbol '(' '/' '*'
		4 - literal		
		*/		

		int begin = 0;
		bool isFloat = false;			
		bool isHex = false;		
		uint i = 0;
		char prev;

		auto finish_number = [&]() {
			if (isFloat)
				tokens.push_back({ fileText.substr(begin, i - begin), NumberFloat });
			else if (isHex)
				tokens.push_back({ fileText.substr(begin, i - begin), NumberHex });
			else
				tokens.push_back({ fileText.substr(begin, i - begin), NumberInt });

			isFloat = false;
			isHex = false;

			begin = i; 
		};

		while (i != fileText.size())
		{
			char ch = fileText[i];

			switch (state)
			{
			case 0:
				if (isalpha(ch))
				{
					begin = i;
					state = 1;
				}
				else if (isdigit(ch))
				{
					begin = i;
					state = 2;
				}
				else if (ch == '"')
				{
					begin = i;
					state = 4;
				}
				else if(!isspace(ch))
				{
					begin = i;
					state = 3;
				}
				break;
			case 1:
				if (isspace(ch))
				{					
					tokens.push_back({ fileText.substr(begin, i - begin), Word });
					state = 0;
				}
				else if (tokens.size() > 0 && (tokens.back().type == NumberFloat || tokens.back().type == NumberInt) && 
					i - begin == 1 && prev == 'x' && isdigit(ch))
				{
					tokens.push_back({ "x", Word });
					begin = i;
					state = 2;
				}
				else if (ch == '"')
				{
					tokens.push_back({ fileText.substr(begin, i - begin), Word });
					begin = i;
					state = 4;
				}
				else if (!isalnum(ch))
				{
					tokens.push_back({ fileText.substr(begin, i - begin), Word });
					begin = i;
					state = 3;
				}
				break;
			case 2:
				if (isspace(ch))
				{
					finish_number();
					state = 0;
				}
				else if (ch == '.')
				{
					if (isFloat || isHex)
						return BadResult("Tokenizing error. Two '.' characters in a number.");
					else					
						isFloat = true;										
				}
				else if (ch == '"')
				{
					finish_number();
					state = 4;
				}
				else if (ch == 'x')
				{
					if (isHex)
					{
						return BadResult("Tokenizing error. Two 'x' characters in a number.");
					}
					else if (i - begin > 1 || prev != '0')
					{
						finish_number();
						state = 1;
					}
					else
						isHex = true;
				}
				else if (!isHex && isalpha(ch))
				{
					finish_number();
					state = 1;
				}
				else if (isHex && (ch < 'a' || ch > 'f') && (ch < 'A' || ch > 'F') && (ch < '0' || ch > '9'))
				{
					return BadResult("Tokenizing error. Invalid hex digit.");					
				}
				else if (!isdigit(ch) && !isHex)
				{
					finish_number();
					state = 3;
				}				
				break;
			case 3:
				if (prev == '-')
				{
					state = 2;
				}
				else
				{
					tokens.push_back({ fileText.substr(begin, i - begin), Special });
					state = 0;
					--i;
				}
				break;
			case 4:
				if (ch == '"')
				{
					tokens.push_back({ fileText.substr(begin + 1, i - begin - 1), Literal });
					state = 0;
				}
				break;			
			}
			prev = ch;
			++i;
		}

		switch (state)
		{
		case 1:
			tokens.push_back({ fileText.substr(begin, i - begin), Word });
			break;
		case 2:
			if (isFloat)
				tokens.push_back({ fileText.substr(begin, i - begin), NumberFloat });
			else
				tokens.push_back({ fileText.substr(begin, i - begin), NumberInt });
			break;
		case 3:
			tokens.push_back({ fileText.substr(begin, i - begin), Word });
		case 100:
			tokens.push_back({ fileText.substr(begin, i - begin), None });
			break;
		default:
			break;
		}
		return Result();
	}
	Result UISceneParser::Evaluate()
	{
		tokenIndex = 0;
		
		while (tokenIndex != tokens.size() - 1)
		{
			EXPECT_WORD("", "Expecting a word in global scope");

			if (COMPARE_VALUE("layer"))
			{
				if (EvaluateLayer()) return BadResult();
			}
			else if (COMPARE_VALUE("Font"))
			{
				if (EvaluateFont()) return BadResult();
			}
			else if (COMPARE_VALUE("element"))
			{
				if (EvaluateElement()) return BadResult();
			}
			else
				return BadResult("Invalid word in global scope: \"" + token.value + "\"");
		}
		return Result();
	}

	Result UISceneParser::EvaluateLayer()
	{
		EXPECT_LITERAL();		

		layerNames[token.value] = scene->CreateLayer();

		return Result();
	}
	Result UISceneParser::EvaluateElement()
	{
		enum class Types {
			Panel,
			Text,
			Button,
			Image
		} type;

		EXPECT_WORD();
		
		if (COMPARE_VALUE("Panel")) type = Types::Panel;
		else if (COMPARE_VALUE("Text")) type = Types::Text;
		else if (COMPARE_VALUE("Button")) type = Types::Button;
		else if (COMPARE_VALUE("Image")) type = Types::Image;
		else return BadResult("Invalid object \"" + token.value + "\"");

		bool hasName = false;
		std::string name;

		EXPECT();

		if (COMPARE_TYPE(Literal))
		{
			name = token.value;
			hasName = true;

			EXPECT();
		}

		CHECK_VALUE("(");

		EXPECT_LITERAL();

		std::string layerName = token.value;

		EXPECT_VALUE(")");

		activeLayer = layerNames[layerName];		

		UIElement* element;
		switch (type)
		{
		case Types::Panel: if (EvaluatePanel(element, name)) return BadResult(); break;
		case Types::Text: if (EvaluateText(element, name)) return BadResult(); break;
		case Types::Button: if (EvaluateButton(element, name)) return BadResult(); break;
		case Types::Image: if (EvaluateImage(element, name)) return BadResult(); break;
		}		

		if (hasName) elementNames[name] = element;
		
		return Result();
	}
	Result UISceneParser::EvaluateFont()
	{
		EXPECT_LITERAL();
		std::string name = token.value;
		EXPECT_WORD();

		Font* font;

		if (COMPARE_VALUE("path"))
		{
			EXPECT_VALUE("=");
			EXPECT_LITERAL();

			String path = String(token.value.data(), token.value.size());

			if (!FileSystem::Exists(path))
				return BadResult();

			font = resourceStorage->CreateResource<Font>();

			font->Load(path);			
		}
		else if (COMPARE_VALUE("name"))
		{
			EXPECT_VALUE("=");
			EXPECT_LITERAL();

			String name = String(token.value.data(), token.value.size());

			auto* manager = resourceStorage->GetResourceManager();
			font = manager->GetResource<Font>(name);
		}
		else if (COMPARE_VALUE("index"))
		{
			EXPECT_VALUE("=");
			EXPECT_INT();

			auto* manager = resourceStorage->GetResourceManager();
			font = manager->GetResource<Font>(token.GetInt());
		}

		fontNames[name] = font;

		EXPECT_VALUE("{");
		EXPECT();
		while (!COMPARE_VALUE("}"))
		{
			CHECK_TYPE(Literal);
			std::string name = token.value;
			EXPECT_WORD();
			std::string renderTypeS = token.value;
			EXPECT_INT();
			int resolution = token.GetInt();

			FontResolutionRenderType renderType;
			if (renderTypeS == "Monochrome") renderType = FontResolutionRenderType::Monochrome;
			else if (renderTypeS == "Antialiased") renderType = FontResolutionRenderType::Antialiased;
			else if (renderTypeS == "LCD") renderType = FontResolutionRenderType::HorizontalLCD;
			else if (renderTypeS == "LCD_v") renderType = FontResolutionRenderType::VerticalLCD;
			else if (renderTypeS == "SDF") renderType = FontResolutionRenderType::SDF;

			FontResolution* res = font->CreateFontResolution(resolution, renderType);
			res->LoadCharacters(0, 127);
			res->CreateAtlas();
			fontResolutionNames[name] = res;

			EXPECT();
		}

		return Result();
	}

	Result UISceneParser::EvaluatePanel(UIElement*& element, std::string name)
	{		
		UI::Panel* panel = scene->CreateElement<UI::Panel>(String(name.data(), name.size()), activeLayer);
		element = panel;

		if (CHECK_NEXT_TOKEN())
			return Result();

		CHECK_VALUE("{");

		EXPECT();		
		while (!COMPARE_VALUE("}"))
		{
			if (COMPARE_VALUE("rect"))
			{
				EXPECT_VALUE("=");
				Rectf rect;
				EvaluateProperty(rect);
				panel->SetRect(rect);
			}
			else if (COMPARE_VALUE("borderWidth"))
			{
				EXPECT_VALUE("=");
				EvaluateProperty(panel->borderWidth);
			}
			else if (COMPARE_VALUE("cornerRadius"))
			{
				EXPECT_VALUE("=");
				EvaluateProperty(panel->cornerRadius);
			}
			else if (COMPARE_VALUE("depth"))
			{
				EXPECT_VALUE("=");
				float depth;
				EvaluateProperty(depth);
				panel->SetDepth(depth);
			}
			else if (COMPARE_VALUE("fillColor"))
			{
				EXPECT_VALUE("=");
				EvaluateProperty(panel->fillColor);
			}
			else if (COMPARE_VALUE("borderColor"))
			{
				EXPECT_VALUE("=");
				EvaluateProperty(panel->borderColor);
			}
			else if (COMPARE_VALUE("localAlignment"))
			{
				EXPECT_VALUE("=");
				Align align;
				EvaluateProperty(align);
				panel->SetLocalAlignment(align);
			}
			else if (COMPARE_VALUE("anchorAlignment"))
			{
				EXPECT_VALUE("=");
				Align align;
				EvaluateProperty(align);
				panel->SetAnchorAlignment(align);
			}
			else if (COMPARE_VALUE("anchor"))
			{
				EXPECT_VALUE("=");
				
				EXPECT_LITERAL();

				if (token.value.size() == 0)
					panel->SetAnchor(nullptr);
				else
				{
					auto it = elementNames.find(token.value);
					if (it != elementNames.end())
						panel->SetAnchor(it->second);
				}
			}
			else if (COMPARE_VALUE("shown"))
			{
				EXPECT_VALUE("=");				
				EvaluateProperty(panel->shown);				
			}
			else if (COMPARE_VALUE("clickable"))
			{
				EXPECT_VALUE("=");
				bool value;
				EvaluateProperty(value);
				panel->SetClickableFlag(value);
			}
			else
				return BadResult();

			EXPECT();			
		}

		return Result();
	}
	Result UISceneParser::EvaluateText(UIElement*& element, std::string name)
	{
		UI::Text* el = scene->CreateElement<UI::Text>(String(name.data(), name.size()), activeLayer);
		element = el;

		if (CHECK_NEXT_TOKEN())
			return Result();

		CHECK_VALUE("{");

		EXPECT();
		while (!COMPARE_VALUE("}"))
		{
			if (COMPARE_VALUE("pos"))
			{
				EXPECT_VALUE("=");
				Vec2f pos;
				EvaluateProperty(pos);
				el->SetPos(pos);
			}
			else if (COMPARE_VALUE("depth"))
			{
				EXPECT_VALUE("=");
				float depth;
				EvaluateProperty(depth);
				el->SetDepth(depth);
			}
			else if (COMPARE_VALUE("localAlignment"))
			{
				EXPECT_VALUE("=");
				Align align;
				EvaluateProperty(align);
				el->SetLocalAlignment(align);
			}
			else if (COMPARE_VALUE("anchorAlignment"))
			{
				EXPECT_VALUE("=");
				Align align;
				EvaluateProperty(align);
				el->SetAnchorAlignment(align);
			}
			else if (COMPARE_VALUE("anchor"))
			{
				EXPECT_VALUE("=");

				EXPECT_LITERAL();

				auto it = elementNames.find(token.value);
				if (it != elementNames.end())
					el->SetAnchor(it->second);
			}
			else if (COMPARE_VALUE("shown"))
			{
				EXPECT_VALUE("=");
				EvaluateProperty(el->shown);
			}
			else if (COMPARE_VALUE("clickable"))
			{
				EXPECT_VALUE("=");
				bool value;
				EvaluateProperty(value);
				el->SetClickableFlag(value);
			}
			else if (COMPARE_VALUE("clipRect"))
			{
				EXPECT_VALUE("=");
				EvaluateProperty(el->clipRect);
			}
			else if (COMPARE_VALUE("color"))
			{
				EXPECT_VALUE("=");
				EvaluateProperty(el->color);
			}
			else if (COMPARE_VALUE("fontSize"))
			{
				EXPECT_VALUE("=");
				EvaluateProperty(el->fontSize);
			}
			else if (COMPARE_VALUE("text"))
			{
				EXPECT_VALUE("=");
				StringUTF8 text;
				EvaluateProperty(text);
				el->SetText(text);
			}
			else if (COMPARE_VALUE("font"))
			{
				EXPECT_VALUE("=");
				EXPECT_LITERAL();
				el->SetFontResolution(fontResolutionNames[token.value]);
				//EXPECT_LITERAL();
				//std::string fontName = token.value;
				//EXPECT_WORD();
				//std::string renderTypeS = token.value;
				//EXPECT_INT();
				//int resolution = token.GetInt();
				//
				//FontResolutionRenderType renderType;
				//if (renderTypeS == "Monochrome") renderType = FontResolutionRenderType::Monochrome;
				//else if (renderTypeS == "Antialiased") renderType = FontResolutionRenderType::Antialiased;
				//else if (renderTypeS == "HorizontalLCD") renderType = FontResolutionRenderType::HorizontalLCD;
				//else if (renderTypeS == "VerticalLCD") renderType = FontResolutionRenderType::VerticalLCD;
				//else if (renderTypeS == "SDF") renderType = FontResolutionRenderType::SDF;
				//
				//Font* font = fontNames[fontName];
				//
				//auto& resolutions = font->GetResolutions();
				//auto it = resolutions.begin();
				//for (; it != resolutions.end(); ++it)
				//	if ((*it)->GetRenderType() == renderType && (*it)->GetResolution() == resolution)
				//		break;
				//
				//el->SetFontResolution(*it);
			}
			else if (COMPARE_VALUE("sizeChanged"))
			{
			EXPECT_VALUE("=");
			EXPECT_LITERAL();
			el->sizeChanged = scene->GetEventFunction({ token.value.data(), token.value.size() });
			}
			else
				return BadResult();

			EXPECT();
		}

		return Result();
	}
	Result UISceneParser::EvaluateButton(UIElement*& element, std::string name)
	{
		UI::Button* el = scene->CreateElement<UI::Button>(String(name.data(), name.size()), activeLayer);
		element = el;

		if (CHECK_NEXT_TOKEN())
			return Result();

		CHECK_VALUE("{");

		EXPECT();
		while (!COMPARE_VALUE("}"))
		{
			if (COMPARE_VALUE("rect"))
			{
				EXPECT_VALUE("=");
				Rectf rect;
				EvaluateProperty(rect);
				el->SetRect(rect);
			}
			else if (COMPARE_VALUE("depth"))
			{
				EXPECT_VALUE("=");
				float depth;
				EvaluateProperty(depth);
				el->SetDepth(depth);
			}
			else if (COMPARE_VALUE("localAlignment"))
			{
				EXPECT_VALUE("=");
				Align align;
				EvaluateProperty(align);
				el->SetLocalAlignment(align);
			}
			else if (COMPARE_VALUE("anchorAlignment"))
			{
				EXPECT_VALUE("=");
				Align align;
				EvaluateProperty(align);
				el->SetAnchorAlignment(align);
			}
			else if (COMPARE_VALUE("anchor"))
			{
				EXPECT_VALUE("=");

				EXPECT_LITERAL();

				auto it = elementNames.find(token.value);
				if (it != elementNames.end())
					el->SetAnchor(it->second);
			}			
			else if (COMPARE_VALUE("clickable"))
			{
				EXPECT_VALUE("=");
				bool value;
				EvaluateProperty(value);
				el->SetClickableFlag(value);
			}											
			else if (COMPARE_VALUE("pressed"))
			{
				EXPECT_VALUE("=");
				EXPECT_LITERAL();
				el->pressed = scene->GetEventFunction({ token.value.data(), token.value.size() });				
			}
			else if (COMPARE_VALUE("released"))
			{
				EXPECT_VALUE("=");
				EXPECT_LITERAL();
				el->released = scene->GetEventFunction({ token.value.data(), token.value.size() });
			}
			else if (COMPARE_VALUE("entered"))
			{
				EXPECT_VALUE("=");
				EXPECT_LITERAL();
				el->entered = scene->GetEventFunction({ token.value.data(), token.value.size() });
			}
			else if (COMPARE_VALUE("left"))
			{
				EXPECT_VALUE("=");
				EXPECT_LITERAL();
				el->left = scene->GetEventFunction({ token.value.data(), token.value.size() });			
			}
			else
				return BadResult();

			EXPECT();
		}

		return Result();
	}
	Result UISceneParser::EvaluateImage(UIElement*& element, std::string name)
	{
		return Result();
	}

	Result UISceneParser::EvaluateProperty(bool& value)
	{		
		EXPECT_VALUE("=");

		EXPECT_WORD();

		if (COMPARE_VALUE("true"))
			value = true;
		else if (COMPARE_VALUE("false"))
			value = false;
	}

	Result UISceneParser::EvaluateProperty(float& value)
	{
		EXPECT_NUMBER();
		value = token.GetFloat();

		return Result();
	}
	Result UISceneParser::EvaluateProperty(ColorRGBAf& value)
	{	
		EXPECT();

		if (COMPARE_VALUE("("))
		{
			EXPECT_INT();
			value.r = (float)token.GetInt() / 255;
			EXPECT_VALUE(",");
			EXPECT_INT();
			value.g = (float)token.GetInt() / 255;
			EXPECT_VALUE(",");
			EXPECT_INT();
			value.b = (float)token.GetInt() / 255;

			EXPECT();			
			if (COMPARE_VALUE(")"))
			{
				value.a = 1.0f;
				return Result();
			}

			CHECK_VALUE(",");

			EXPECT_INT();
			value.a = (float)token.GetInt() / 255;

			EXPECT_VALUE(")");

			return Result();
		}
		else if (COMPARE_TYPE(NumberHex) || COMPARE_TYPE(NumberInt))
		{
			value = token.GetInt();
		}

		return Result();
	}
	Result UISceneParser::EvaluateProperty(Align& value)
	{
		EXPECT_WORD();

		if (COMPARE_VALUE("Top")) value = Align::Top;
		else if (COMPARE_VALUE("TopRight")) value = Align::TopRight;
		else if (COMPARE_VALUE("Right")) value = Align::Right;
		else if (COMPARE_VALUE("BottomRight")) value = Align::BottomRight;
		else if (COMPARE_VALUE("Bottom")) value = Align::Bottom;
		else if (COMPARE_VALUE("BottomLeft")) value = Align::BottomLeft;
		else if (COMPARE_VALUE("Left")) value = Align::Left;
		else if (COMPARE_VALUE("TopLeft")) value = Align::TopLeft;
		else if (COMPARE_VALUE("Center")) value = Align::Center;
		else
			return BadResult();

		return Result();
	}
	Result UISceneParser::EvaluateProperty(Vec2f& value)
	{
		EXPECT_VALUE("(");

		EXPECT_NUMBER();
		value.x = token.GetFloat();

		EXPECT_VALUE(",");

		EXPECT_NUMBER();
		value.y = token.GetFloat();

		EXPECT_VALUE(")");

		return Result();
	}
	Result UISceneParser::EvaluateProperty(Rectf& rect)
	{
		EXPECT_VALUE("(");	

		EXPECT_NUMBER();
		rect.x = token.GetFloat();

		EXPECT_VALUE(",");

		EXPECT_NUMBER();
		rect.y = token.GetFloat();

		EXPECT_VALUE(")");

		EXPECT_NUMBER();		
		rect.w = token.GetFloat();

		EXPECT_VALUE("x");

		EXPECT_NUMBER();
		rect.h = token.GetFloat();

		return Result();
	}
	Result UISceneParser::EvaluateProperty(StringUTF8& value)
	{
		EXPECT_LITERAL();

		value = StringUTF8(token.value.data(), token.value.size());

		return Result();
	}

	Result UISceneParser::Parse(UIScene* scene, Resource::ResourceStorage* storage, const Path& path)
	{
		this->resourceStorage = storage;
		this->scene = scene;
		
		LoadFile(path);
		RemoveComments();
		if (Tokenize()) return Result();
		if (Evaluate()) return Result();
		//std::stringstream ss{ std::string((char*)buffer.Ptr(), buffer.Size()) };
		//std::string line;
		//
		//while (std::getline(ss, line))
		//{
		//	args = Split(line);
		//
		//	if (args.size() == 0)
		//		continue;
		//
		//	if (args[0] == "Layer")
		//		ParseLayer();
		//	else if (args[0] == "Panel")
		//		ParsePanel();
		//}

		return Result();
	}

	float UISceneParser::Token::GetFloat()
	{
		if (type == NumberHex)		
			return stoul(value);		
		else if (type == NumberInt)		
			return stoi(value);		
		else
			return stof(value);
	}
	uint32 UISceneParser::Token::GetInt()
	{
		if (type == NumberHex)
			return std::stoul(value, nullptr, 16);
		else if (type == NumberInt)
			return stoi(value);		
	}
}