#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Graphics/GraphicsLibrary.h"
#include <vector>

namespace Blaze
{
	class Engine;
}
namespace Blaze::UI
{
	class State;
	class Element;
	class Text;
	class Panel;
	class ButtonState;
	class TextFieldState;
	class BLAZE_API Layer
	{				
	protected:
		virtual void Setup() { }
		virtual void Update() { }
		virtual void NewViewport(Vec2i size) { }
		virtual void Activated() { }		

		void Add(Text&);
		void Add(Panel&);
		void Add(ButtonState&);
		void Add(TextFieldState&);		
		
		State* selected;
		std::vector<State*> states;
		std::vector<Element*> elements;

		struct TextData {
			Graphics::Core::Program program;
			Graphics::Core::GraphicsBuffer vb;
			Graphics::Core::VertexArray va;
			int uniformTex;
			int uniformVP;

			std::vector<Text*> texts;
		} text;		
		struct PanelData {
			Graphics::Core::VertexArray va;
			Graphics::Core::GraphicsBuffer vb;
			Graphics::Core::Program program;
			int uniformVP;

			std::vector<Panel*> panels;			
		} panel;
		struct ButtonStateData {
			std::vector<ButtonState*> buttons;
		} buttonStateData;
		struct TextFieldStateData{
			std::vector<TextFieldState*> textFields;
		} textFieldState;

		bool active = true;
		bool interactable = true;
	public:
		Layer();

		void SetActive(bool active);
		void SetInteractable(bool interactable);		
		
		friend class Element;
		friend class State;
		friend class Blaze::Engine;
	};
}