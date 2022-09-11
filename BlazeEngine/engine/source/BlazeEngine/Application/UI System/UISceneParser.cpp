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
				if (Result r = EvaluateLayer()) return r;
			}
			else if (COMPARE_VALUE("Font"))
			{
				if (Result r = EvaluateFont()) return r;
			}
			else if (COMPARE_VALUE("Texture2D"))
			{
				if (Result r = EvaluateTexture2D()) return r;
			}
			else if (COMPARE_VALUE("element"))
			{
				if (Result r = EvaluateElement()) return r;
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
	Result UISceneParser::EvaluateTexture2D()
	{
		EXPECT_LITERAL();
		std::string name = token.value;
		EXPECT_WORD();

		Graphics::Core::Texture2D* tex;

		if (COMPARE_VALUE("path"))
		{
			EXPECT_VALUE("=");
			EXPECT_LITERAL();

			String path = String(token.value.data(), token.value.size());

			if (!FileSystem::Exists(path))
				return BadResult();

			auto res = resourceStorage->CreateResource<Graphics::Core::Texture2D>(name.c_str());			

			if (res)			
				return res.result;			

			tex = res.value;

			tex->Load(path);
		}
		else if (COMPARE_VALUE("name"))
		{
			EXPECT_VALUE("=");
			EXPECT_LITERAL();

			String name = String(token.value.data(), token.value.size());

			auto* manager = resourceStorage->GetResourceManager();

			if (manager == nullptr)
				return BadResult("No resource manager set");

			tex = manager->GetResource<Graphics::Core::Texture2D>(name);

			if (tex == nullptr)
				return BadResult("Theres no font resource with a given name");
		}
		else if (COMPARE_VALUE("index"))
		{
			EXPECT_VALUE("=");
			EXPECT_INT();

			auto* manager = resourceStorage->GetResourceManager();

			if (manager == nullptr)
				return BadResult("No resource manager set");

			tex = manager->GetResource<Graphics::Core::Texture2D>(token.GetInt());

			if (tex == nullptr)
				return BadResult("Theres no font resource with a given index");
		}

		texture2DNames[name] = tex;

		Graphics::Core::Texture2DSettings settings;

		if (!CHECK_NEXT_TOKEN())
			if (COMPARE_VALUE("{"))
			{

				EXPECT();
				while (!COMPARE_VALUE("}"))
				{
					CHECK_TYPE(Word);
					std::string name = token.value;

					EXPECT_VALUE("=");

					EXPECT_WORD();
					std::string value = token.value;

					if (name == "min")
					{
						if (value == "Linear")
							settings.min = Graphics::Core::TextureSampling::Linear;
						else if (value == "Nearest")
							settings.min = Graphics::Core::TextureSampling::Nearest;
						else
							return BadResult("Invalid texture filtering value");
					}
					else if (name == "mag")
					{
						if (value == "Linear")
							settings.mag = Graphics::Core::TextureSampling::Linear;
						else if (value == "Nearest")
							settings.mag = Graphics::Core::TextureSampling::Nearest;
						else
							return BadResult("Invalid texture filtering value");
					}
					else if (name == "mip")
					{
						if (value == "Linear")
							settings.mip = Graphics::Core::TextureSampling::Linear;
						else if (value == "Nearest")
							settings.mip = Graphics::Core::TextureSampling::Nearest;
						else
							return BadResult("Invalid texture filtering value");
					}
					else if (name == "xWrap")
					{
						if (value == "ClampToBorder")
							settings.xWrap = Graphics::Core::TextureWrapping::ClampToBorder;
						else if (value == "ClampToEdge")
							settings.xWrap = Graphics::Core::TextureWrapping::ClampToEdge;
						else if (value == "MirroredRepeat")
							settings.xWrap = Graphics::Core::TextureWrapping::MirroredRepeat;
						else if (value == "Repeat")
							settings.xWrap = Graphics::Core::TextureWrapping::Repeat;
						else
							return BadResult("Invalid texture wrapping value");
					}
					else if (name == "yWrap")
					{
						if (value == "ClampToBorder")
							settings.yWrap = Graphics::Core::TextureWrapping::ClampToBorder;
						else if (value == "ClampToEdge")
							settings.yWrap = Graphics::Core::TextureWrapping::ClampToEdge;
						else if (value == "MirroredRepeat")
							settings.yWrap = Graphics::Core::TextureWrapping::MirroredRepeat;
						else if (value == "Repeat")
							settings.yWrap = Graphics::Core::TextureWrapping::Repeat;
						else
							return BadResult("Invalid texture wrapping value");
					}
					else if (name == "mipmaps")
					{
						if (value == "true")
							settings.mipmaps = true;
						else if (value == "false")
							settings.mipmaps = false;
						else
							return BadResult("Invalid texture mipmap flag value");
					}
					EXPECT();
				}
			}
			else
				ReverseToken();		

		tex->SetSettings(settings);		
		return Result();
	}
	Result UISceneParser::EvaluateElement()
	{
		EXPECT_WORD();
				
		int typeIndex = registry.GetElementTypeIndex(StringView(token.value.c_str()));
		auto& typeData = registry.GetElementTypeData(typeIndex);

		if (typeIndex == -1)		
			return BadResult("Invalid element type");		

		if (this->parsingData == nullptr)
			return BadResult("No parsing data set");				

		const auto& parsingData = this->parsingData[typeIndex];

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

		if (hasName)
		{
			auto result = scene->CreateElement(name.c_str(), typeIndex, activeLayer);

			if (result)
				return result.result;

			element = result.value;
		}
		else
			element = scene->CreateElement(typeIndex, activeLayer);
		
		if (CHECK_NEXT_TOKEN())		
			return Result();

		if (!COMPARE_VALUE("{"))
		{
			ReverseToken();
			return Result();
		}

		EXPECT();
		while (!COMPARE_VALUE("}"))
		{
			EvalueateUIElementMember(element, typeData, parsingData);

			EXPECT();
		}		

		
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

			if (manager == nullptr)
				return BadResult("No resource manager set");

			font = manager->GetResource<Font>(name);

			if (font == nullptr)
				return BadResult("Theres no font resource with a given name");
		}
		else if (COMPARE_VALUE("index"))
		{
			EXPECT_VALUE("=");
			EXPECT_INT();

			auto* manager = resourceStorage->GetResourceManager();

			if (manager == nullptr)
				return BadResult("No resource manager set");

			font = manager->GetResource<Font>(token.GetInt());

			if (font == nullptr)
				return BadResult("Theres no font resource with a given index");
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

			scene->GetResourceManager()->AddResource<FontResolution>(name.c_str(), res);

			fontResolutionNames[name] = res;

			EXPECT();
		}

		return Result();
	}

	Result UISceneParser::EvalueateUIElementMember(UIElement* element, const UIElementTypeData& typeData, const UIElementParsingData& parsingData)
	{
		std::string memberTypeName = token.value;
		int memberIndex = parsingData.GetMemberIndex(token.value.c_str());

		EXPECT("=");

		if (memberIndex != -1)
		{			
			auto memberTypeData = parsingData.GetMember(memberIndex).GetTypeData();
			switch (memberTypeData.type)
			{
			case UIElementMemberTypeData::UIElementMemberType::Int:			EvaluateUIElementMemberType<int>		(element, parsingData, memberIndex, memberTypeData.list); break;			
			case UIElementMemberTypeData::UIElementMemberType::Float:		EvaluateUIElementMemberType<float>		(element, parsingData, memberIndex, memberTypeData.list); break;
			case UIElementMemberTypeData::UIElementMemberType::Bool:		EvaluateUIElementMemberType<bool>		(element, parsingData, memberIndex, memberTypeData.list); break;
			case UIElementMemberTypeData::UIElementMemberType::Vec2f:		EvaluateUIElementMemberType<Vec2f>		(element, parsingData, memberIndex, memberTypeData.list); break;
			case UIElementMemberTypeData::UIElementMemberType::Vec3f:		EvaluateUIElementMemberType<Vec3f>		(element, parsingData, memberIndex, memberTypeData.list); break;
			case UIElementMemberTypeData::UIElementMemberType::ColorRGBf:	EvaluateUIElementMemberType<ColorRGBf>	(element, parsingData, memberIndex, memberTypeData.list); break;
			case UIElementMemberTypeData::UIElementMemberType::ColorRGBAf:	EvaluateUIElementMemberType<ColorRGBAf>	(element, parsingData, memberIndex, memberTypeData.list); break;
			case UIElementMemberTypeData::UIElementMemberType::Align:		EvaluateUIElementMemberType<Align>		(element, parsingData, memberIndex, memberTypeData.list); break;
			case UIElementMemberTypeData::UIElementMemberType::Rectf:		EvaluateUIElementMemberType<Rectf>		(element, parsingData, memberIndex, memberTypeData.list); break;
			case UIElementMemberTypeData::UIElementMemberType::String:		EvaluateUIElementMemberType<String>		(element, parsingData, memberIndex, memberTypeData.list); break;
			case UIElementMemberTypeData::UIElementMemberType::StringUTF8:	EvaluateUIElementMemberType<StringUTF8>	(element, parsingData, memberIndex, memberTypeData.list); break;
			default: {
				return BadResult("Invalid element member");
				break;
			}
			}
		}
		else
			return BadResult("Invalid element member name");

		return Result();
	}

	template<typename T>
	Result UISceneParser::EvaluateUIElementMemberType(UIElement* element, const UIElementParsingData& parsingData, uint memberIndex, bool isList)
	{
		if (isList)
		{
			std::vector<T> value;

			EvaluateList<T>(value);

			if (auto r = parsingData.SetValue<std::vector<T>>(memberIndex, scene, element, value))
				return r;
		}
		else
		{
			T value;

			EvaluateProperty<T>(value);

			if (auto r = parsingData.SetValue<T>(memberIndex, scene, element, value))
				return r;
		}

		return Result();
	}

	template<> Result UISceneParser::EvaluateProperty(int& value)
	{
		EXPECT_INT();
		value = token.GetInt();

		return Result();
	}
	template<> Result UISceneParser::EvaluateProperty(float& value)
	{
		EXPECT_NUMBER();
		value = token.GetFloat();

		return Result();
	}	
	template<> Result UISceneParser::EvaluateProperty(bool& value)
	{		
		EXPECT_VALUE("=");

		EXPECT_WORD();

		if (COMPARE_VALUE("true"))
			value = true;
		else if (COMPARE_VALUE("false"))
			value = false;
	}
	template<> Result UISceneParser::EvaluateProperty(Vec2f& value)
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
	template<> Result UISceneParser::EvaluateProperty(Vec3f& value)
	{
		EXPECT_VALUE("(");

		EXPECT_NUMBER();
		value.x = token.GetFloat();

		EXPECT_VALUE(",");

		EXPECT_NUMBER();
		value.y = token.GetFloat();

		EXPECT_VALUE(",");

		EXPECT_NUMBER();
		value.z = token.GetFloat();

		EXPECT_VALUE(")");

		return Result();
	}
	template<> Result UISceneParser::EvaluateProperty(ColorRGBf& value)
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

			EXPECT_VALUE(")");			
			return Result();						
		}
		else if (COMPARE_TYPE(NumberHex) || COMPARE_TYPE(NumberInt))
		{
			value = token.GetInt();
		}

		return Result();
	}
	template<> Result UISceneParser::EvaluateProperty(ColorRGBAf& value)
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
	template<> Result UISceneParser::EvaluateProperty(Rectf& rect)
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
	template<> Result UISceneParser::EvaluateProperty(Align& value)
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
	template<> Result UISceneParser::EvaluateProperty(String& value)
	{
		EXPECT_LITERAL();

		value = String(token.value.data(), token.value.size());

		return Result();
	}
	template<> Result UISceneParser::EvaluateProperty(StringUTF8& value)
	{
		EXPECT_LITERAL();

		value = StringUTF8(token.value.data(), token.value.size());

		return Result();
	}

	template<typename T>
	Result UISceneParser::EvaluateList(std::vector<T>& vector)
	{
		EXPECT_VALUE("{");
		
		T val;						
		
		EXPECT();
		if (COMPARE_VALUE("}"))
			return Result();
		ReverseToken();

		while (true)
		{
			EvaluateProperty<T>(val);
			vector.emplace_back(val);

			EXPECT();
			if (!COMPARE_VALUE(","))
				if (COMPARE_VALUE("}"))
					break;
				else
					return BadResult("Expected a '}' or ',' inside a list");
		}

		return Result();
	}

	Result UISceneParser::Parse(UIScene* scene, Resource::ResourceStorage* storage, UIElementParsingData* parsingData, const Path& path)
	{
		this->parsingData = parsingData;
		this->registry = scene->GetManager().GetElementTypeRegistry();
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