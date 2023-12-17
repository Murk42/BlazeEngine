#pragma once
#include "Resources/ResourceManager.h"

namespace UI2
{				
	class Node;
	class Screen;

	struct NodeTransform
	{
		Vec2f pos;
		Vec2f parentPivot;
		Vec2f pivot;
		Vec2f size;
		Vec2f scale;
		float rotation;
	};

	struct NodeTransformUpdatedEvent
	{
		Node* node;
	};
	struct NodePreTransformUpdatedEvent
	{
		Node* node;
	};

	class Node
	{
	public:	
		VirtualMap<String> otherData;
		EventDispatcher<NodeTransformUpdatedEvent> transformUpdatedEventDipatcher;
		EventDispatcher<NodePreTransformUpdatedEvent> transformPreUpdatedEventDipatcher;

		Node(Node* parent, const NodeTransform& transform);
		virtual ~Node();

		void SetTransform(const NodeTransform& transform);		
	
		virtual String GetTypeName() const = 0;		

		ArrayView<Node*> GetChildren() const { return children; }
		Node* GetParent() const { return parent; }
		Screen* GetScreen() const { return screen; }
		inline NodeTransform GetTransform() const { return transform; }

		Vec2f GetFinalPosition();
		Vec2f GetFinalSize();
		float GetFinalRotation();
	private:		
		Screen* screen;
		Node* parent;				
		Array<Node*> children;

		NodeTransform transform;
		bool transformDirty;
		Vec2f finalScale;
		Vec2f finalSize;
		Vec2f finalPosition;
		float finalRotation;		

		void MakeTransformDirty();

		void CalculateTransformRecursive();
		void CalculateTransform();
	
		friend class Screen;
	};		
	
	class Button : public Node
	{
	public:
		String GetTypeName() const { return "Button"; }
	};
	
	class Image : public Node
	{
	public:				
		Graphics::OpenGLWrapper::Texture2D* texture;
		Vec2f uv1, uv2;
		ColorRGBAf color;
		float blend, alpha;

		Image(Node* parent, const NodeTransform& transform);

		String GetTypeName() const { return "Image"; }
	};
	
	class Panel : public Node
	{
	public:		
		ColorRGBAf fillColor, borderColor;
		float cornerRadius, borderWidth;

		Panel(Node* parent, const NodeTransform& transform);

		String GetTypeName() const { return "Panel"; }
	};	
	
	class Text : public Node, private EventHandler<NodePreTransformUpdatedEvent>
	{
	public:		
		struct CharacterData
		{
			UnicodeChar character;
			ColorRGBAf color;
			Vec2i pos;
			Vec2u size;
		};				

		Text(Node* parent, const NodeTransform& transform);
		~Text();
		
		template<typename T> 
		void SetTextLayouter() 
		{ 
			delete layouter;
			layouter = new T(); 
			
			GenerateCharacterData();			
		}
		void SetText(StringUTF8 text);
		void SetTextColors(Array<ColorRGBAf> colors);
		void SetFont(Font& font);
		
		void SetTextWidth(float width);
		void SetTextHeight(float height);
		void SetTextSize(Vec2f size);
		void SetFontHeight(uint pixelFontHeight);		

		inline String GetTypeName() const { return "Text"; }
		inline Font* GetFont() const { return font; }
		inline StringViewUTF8 GetText() const { return text; }
		inline uint GetPixelFontHeight() const { return pixelFontHeight; }
		inline ArrayView<CharacterData> GetCharacterData() const { return characterData; };
	private:				
		enum SizingType
		{
			ByTextWidth,
			ByTextHeight,
			ByTextSize,
			ByFontHeight,
		};

		TextLayouterBase* layouter;
		StringUTF8 text;
		Array<ColorRGBAf> colors;
		Font* font;	

		//FontMetrics* fontMetrics;		
		Array<CharacterData> characterData;		
		Vec2f textSize;
		uint pixelFontHeight;		
		SizingType sizingType;	
		bool dirty;

		void GenerateCharacterData();
		void GenerateCharacterDataByTextWidth();
		void GenerateCharacterDataByTextHeight();
		void GenerateCharacterDataByTextSize();
		void GenerateCharacterDataByFontHeight();		

		void CopyCharacterDataFromLayouter(Vec2f scaling);

		void UpdateSize();
		void OnEvent(NodePreTransformUpdatedEvent event) override;
	};	

	struct NodeCreatedEvent
	{		
		Node* node;
	};
	struct NodeDestroyedEvent
	{		
		Node* node;
	};
	struct ScreenDestructionEvent
	{
		Screen* screen;
	};
	
	class Screen : public Node
	{
	public:	
		Blaze::EventDispatcher<NodeCreatedEvent> nodeCreatedEventDispatcher;
		Blaze::EventDispatcher<NodeDestroyedEvent> nodeDestroyedEventDispatcher;
		Blaze::EventDispatcher<ScreenDestructionEvent> screenDestructionEventDispatcher;

		Screen();
		virtual ~Screen();

		void CalculateAllTransforms();

		virtual String GetTypeName() const { return "Screen"; };
	private:			
		void CalculateAllTransforms(Node* node);
		void AddNode(Node* node);
		void RemoveNode(Node* node);
	
		friend class Node;
	};				
}
