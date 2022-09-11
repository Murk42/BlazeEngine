#pragma once
#include "BlazeEngine/Application/UI System/UIScene.h"
#include <sstream>
#include <unordered_map>

namespace Blaze::UI
{
	class UISceneParser
	{				
		UIScene* scene;
		UIElementTypeRegistry registry;
		Resource::ResourceStorage* resourceStorage;
		UIElementParsingData* parsingData;

		std::string fileText;
		std::unordered_map<std::string, uint> layerNames;
		std::unordered_map<std::string, Font*> fontNames;
		std::unordered_map<std::string, FontResolution*> fontResolutionNames;
		std::unordered_map<std::string, Graphics::Core::Texture2D*> texture2DNames;

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
		Result EvaluateTexture2D();
		Result EvaluateElement();	
		Result EvaluateFont();		

		Result EvalueateUIElementMember(UIElement* element, const UIElementTypeData&, const UIElementParsingData&);
		template<typename T>
		Result EvaluateUIElementMemberType(UIElement* element, const UIElementParsingData&, uint memberIndex, bool isList);

		template<typename T>
		Result EvaluateProperty(T&);
		template<> Result EvaluateProperty(int& value);
		template<> Result EvaluateProperty(float& value);		
		template<> Result EvaluateProperty(bool& value);
		template<> Result EvaluateProperty(Vec2f& value);
		template<> Result EvaluateProperty(Vec3f& value);
		template<> Result EvaluateProperty(ColorRGBf& value);
		template<> Result EvaluateProperty(ColorRGBAf& value);
		template<> Result EvaluateProperty(Rectf& rect);
		template<> Result EvaluateProperty(Align& value);
		template<> Result EvaluateProperty(String& value);
		template<> Result EvaluateProperty(StringUTF8& value);
		
		template<typename T>
		Result EvaluateList(std::vector<T>& vector);
	public:

		Result Parse(UIScene* scene, Resource::ResourceStorage* storage, UIElementParsingData* parsingData, const Path& path);
	};
}