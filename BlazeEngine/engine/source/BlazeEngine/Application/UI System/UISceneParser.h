#pragma once
#include "BlazeEngine/Application/UI System/UIScene.h"
#include <sstream>
#include <unordered_map>

namespace Blaze::UI
{
	class UISceneParser
	{				
		UIScene* scene;
		Resource::ResourceStorage* resourceStorage;
		std::string fileText;					
		std::unordered_map<std::string, uint> layerNames;
		std::unordered_map<std::string, UIElement*> elementNames;
		std::unordered_map<std::string, Font*> fontNames;
		std::unordered_map<std::string, FontResolution*> fontResolutionNames;

		enum TokenType
		{
			Word,
			NumberHex,
			NumberInt,
			NumberFloat,
			Special,
			Literal,
			None
		};
		struct Token
		{
			std::string value;
			TokenType type;

			float GetFloat();
			uint32 GetInt();
		};
		std::vector<Token> tokens;
		Token token;
		uint tokenIndex = 0;		

		uint activeLayer;

		bool GetNextToken();
		bool GetNextToken(std::string);
		void ReverseToken();

		Result LoadFile(Path path);
		Result RemoveComments();
		Result Tokenize();
		Result Evaluate();

		Result EvaluateLayer();
		Result EvaluateElement();	
		Result EvaluateFont();

		Result EvaluatePanel(UIElement*& element, std::string name);
		Result EvaluateText(UIElement*& element, std::string name);
		Result EvaluateButton(UIElement*& element, std::string name);
		Result EvaluateImage(UIElement*& element, std::string name);

		Result EvaluateProperty(bool& value);
		Result EvaluateProperty(float& value);
		Result EvaluateProperty(ColorRGBAf& value);
		Result EvaluateProperty(Align& value);
		Result EvaluateProperty(Vec2f& value);
		Result EvaluateProperty(Rectf& rect);
		Result EvaluateProperty(StringUTF8& value);
	public:

		Result Parse(UIScene* scene, Resource::ResourceStorage* storage, const Path& path);
		/**		

		Result ParseLayer()
		{
			if (args.size() > 1)
			{
				std::string layerName;
				StripQuotes(args[1], layerName);
				layerNames.insert({ layerName, scene->CreateLayer() });
			}
			else return Result(Log(LogType::Error, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, "", ""));

			return Result();
		}

		Result ParsePanel()
		{
			if (args.size() >= 3)
			{
				std::string name;
				StripQuotes(args[2], name);

				std::string layerName;
				StripQuotes(args[2], layerName);
				UI::Panel* panel = scene->CreateElement<UI::Panel>(layerNames[layerName]);

				for (int i = 2; i < args.size(); ++i)
				{
					symbols = SplitSymbols(args[i]);

					if (symbols[0] == "borderColor") ParseProperty(panel->borderColor);
					else if (symbols[0] == "borderWidth") ParseProperty(panel->borderWidth);
					else if (symbols[0] == "clickable") ParseProperty(panel->clickable);
					else if (symbols[0] == "cornerRadius") ParseProperty(panel->cornerRadius);
					else if (symbols[0] == "depth") ParseProperty(panel->depth);
					else if (symbols[0] == "fillColor") ParseProperty(panel->fillColor);
					else if (symbols[0] == "rect") ParseProperty(panel->rect);
					else if (symbols[0] == "shown") ParseProperty(panel->shown);
					else if (symbols[0] == "localAlignment") ParseProperty(panel->localAlignment);
					else if (symbols[0] == "anchorAlignment") ParseProperty(panel->anchorAlignment);
					else return Result(Log(LogType::Error, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, "", ""));
				}
			}
			else return Result(Log(LogType::Error, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, "", ""));

			return Result();
		}

		Result ParseProperty(bool& value)
		{
			if (symbols.size() != 3)
				return Result(Log(LogType::Error, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, "", ""));

			value = (stoi(symbols[2]) != 0);

			return Result();
		}
		Result ParseProperty(float& value)
		{
			if (symbols.size() != 3)
				return Result(Log(LogType::Error, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, "", ""));

			value = stof(symbols[2]);

			return Result();
		}
		Result ParseProperty(Vec2f& value)
		{
			if (symbols.size() != 5)
				return Result(Log(LogType::Error, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, "", ""));

			value.x = stof(symbols[2]);
			value.y = stof(symbols[4]);

			return Result();
		}
		Result ParseProperty(Rectf& value)
		{
			if (symbols.size() != 9)
				return Result(Log(LogType::Error, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, "", ""));

			value.x = stof(symbols[2]);
			value.y = stof(symbols[4]);
			value.w = stof(symbols[6]);
			value.h = stof(symbols[8]);

			return Result();
		}
		Result ParseProperty(ColorRGBAf& value)
		{
			if (symbols.size() != 9)
				return Result(Log(LogType::Error, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, "", ""));

			value.r = stof(symbols[2]);
			value.g = stof(symbols[4]);
			value.b = stof(symbols[6]);
			value.a = stof(symbols[8]);

			return Result();
		}
		Result ParseProperty(Align& value)
		{
			if (symbols.size() != 3)
				return Result(Log(LogType::Error, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, "", ""));

			if (symbols[2] == "TopLeft")
				value = Align::TopLeft;
			else if (symbols[2] == "BottomLeft")
				value = Align::BottomLeft;
			else if (symbols[2] == "TopRight")
				value = Align::TopRight;
			else if (symbols[2] == "BottomRight")
				value = Align::BottomRight;
			else if (symbols[2] == "Center")
				value = Align::Center;
			else if (symbols[2] == "Left")
				value = Align::Left;
			else if (symbols[2] == "Right")
				value = Align::Right;
			else if (symbols[2] == "Top")
				value = Align::Top;
			else if (symbols[2] == "Bottom")
				value = Align::Bottom;

			return Result();
		}
	*/
	};
}