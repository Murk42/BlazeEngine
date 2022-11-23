#pragma once

namespace Blaze::UI2
{
	class ElementManager;
	class Scene;

	class Element
	{
		uint typeIndex;
		uint childCount;

		void* derived;
	public:
		String name;
		Align anchorAlign = Align::Center;
		Align localAlign = Align::Center;
		Vec2f size = Vec2f();
		Vec2f offset = Vec2f();
		bool active = true;
		Rectf clipRect = Rectf(0, 0, NAN, NAN);
		Scene* scene;

		Vec2f position;

		using ManagerType = ElementManager;
		static constexpr const char* typeName = "Element";

		friend class Scene;
		friend class ElementManager;
	};
	
	class ElementManager
	{
	public:
		virtual bool ParseProperty(Element*, void*, StringView name, StringView value);
		virtual void Render(Element*, void*);
	};	

	class PanelManager;

	struct Panel
	{
		float borderWidth;
		float cornerRadius;
		ColorRGBAf borderColor;
		ColorRGBAf color;

		using ManagerType = PanelManager;
		static constexpr const char* typeName = "Panel";
	};

	class PanelManager : public ElementManager
	{
		Graphics::Core::VertexArray panelsVA;
		Graphics::Core::ShaderProgram panelsSP;
		Graphics::Core::GraphicsBuffer panelsVB;
	public:
		PanelManager();

		bool ParseProperty(Element*, void*, StringView name, StringView value);
		void Render(Element*, void*);
	};
}