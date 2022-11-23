#include "Scene.h"
#include "SceneFileParser.h"
#include "Scene.h"

namespace Blaze::UI2
{
	Vec2f GetAlignOffset(Align align, Vec2f size)
	{
		switch (align)
		{
		case Blaze::Align::Top:			return Vec2f(size.x / 2	, size.y	);
		case Blaze::Align::TopRight:	return Vec2f(size.x		, size.y	);
		case Blaze::Align::Right:		return Vec2f(size.x		, size.y / 2);
		case Blaze::Align::BottomRight:	return Vec2f(size.x		, 0			);
		case Blaze::Align::Bottom:		return Vec2f(size.x / 2	, 0			);
		case Blaze::Align::BottomLeft:	return Vec2f(0			, 0			);
		case Blaze::Align::Left:		return Vec2f(0			, size.y / 2);
		case Blaze::Align::TopLeft:		return Vec2f(0			, size.y	);
		case Blaze::Align::Center:		return Vec2f(size.x / 2	, size.y / 2);
		}
	}

	Scene::Scene()
		: manager(nullptr)
	{		
	}
	Scene::~Scene()
	{
		Clear();
	}

	void Scene::Clear()
	{
		elements.clear();

		for (auto& container : derivedElements)
		{
			Memory::Free(container.ptr);
			container.ptr = nullptr;
			container.size = 0;
		}
	}

	void Scene::SetManager(Manager* manager)
	{
		this->manager = manager;
		auto& registry = manager->GetRegistry();
		size_t typeCount = registry.GetElementTypeCount();
		derivedElements.Resize(typeCount);

		for (int i = 0; i < typeCount; ++i)
		{
			derivedElements[i].size = 0;
			derivedElements[i].ptr = nullptr;
			derivedElements[i].typeData = registry.GetElementTypeData(i);
		}
	}
	void* Scene::CreateDerivedElement(size_t typeIndex)
	{		
		auto& container = derivedElements[typeIndex];
		void* old = container.ptr;
		size_t oldSize = container.size;
		container.size += container.typeData.size;
		container.ptr = Memory::Allocate(container.size);		
		memcpy(container.ptr, old, oldSize);
		Memory::Free(old);

		void* newElement = (char*)container.ptr + oldSize;
		container.typeData.construct(newElement);
		return newElement;
	}
	void Scene::CreateElement(SceneFileParser::Element& data, Result& r)
	{	
		auto& registry = manager->GetRegistry();
		int typeIndex = registry.GetElementTypeIndex(data.typeName);

		if (typeIndex == -1)
		{
			r.AddLog(BLAZE_WARNING_LOG("Blaze Engine", "Invalid element type name: \"" + data.typeName + "\""));
			r.SetFailed(true);
			return;
		}

		ElementManager* elementManager = manager->GetElementManager(typeIndex);

		void* derived = nullptr;
		if (typeIndex != 0)
			derived = CreateDerivedElement(typeIndex);

		Element& e = elements.emplace_back();
		e.childCount = data.children.size();
		e.typeIndex = typeIndex;		
		e.derived = derived;
		e.scene = this;
				
		for (auto& p : data.properties)
			if (!elementManager->ParseProperty(&e, derived, p.name, p.value))
				r.AddLog(BLAZE_WARNING_LOG("Blaze Engine", "Unknown property name: \"" + p.name + "\""));

		for (auto& child : data.children)
		{
			CreateElement(child, r);
			if (r)
				return;	
		}
	}

	void Scene::UpdateElement(uint& index, Element* parent)
	{
		Element& element = elements[index];

		if (parent != nullptr)
		{
			element.position =
				parent->position + 
				element.offset +
				GetAlignOffset(element.anchorAlign, parent->size) -
				GetAlignOffset(element.localAlign, element.size);
		}
		else
		{
			element.position = 
				element.offset +
				GetAlignOffset(element.anchorAlign, (Vec2f)Graphics::Core::GetViewportSize()) -
				GetAlignOffset(element.localAlign, element.size);
		}

		++index;

		for (int i = 0; i < element.childCount; ++i)
			UpdateElement(index, &element);
	}

	void Scene::Update()
	{		
		for (uint index = 0; index < elements.size();)
			UpdateElement(index, nullptr);
	}

	void Scene::Render()
	{
		for (uint i = 0; i < elements.size(); ++i)
		{	
			ElementManager* em = manager->GetElementManager(elements[i].typeIndex);
			void* derived = elements[i].derived;

			em->Render(&elements[i], derived);
		}
	}

	Result Scene::Load(Path path)
	{
		SceneFileParser parser;
		if (Result r = parser.Load(path)) return r;

		elements.reserve(parser.elementCount);

		Result r;

		for (auto& e : parser.elements)
		{
			CreateElement(e, r);

			if (r)
				break;
		}

		if (r)
			Clear();
		
		return r;
	}

	Manager::Manager()
	{
	}

	Manager::~Manager()
	{
		for (uint i = 0; i < registry.GetElementTypeCount(); ++i)
		{
			auto data = registry.GetElementTypeData(i);
			data.destructManager(managers[i]);
			Memory::Free(managers[i]);
		}
	}

	ElementManager* Manager::GetElementManager(uint typeIndex)
	{
		return managers[typeIndex];
	}

	Result Manager::SetElementTypeRegistry(const ElementTypeRegistry& r)
	{
		if (registry.GetElementTypeCount() != 0)
			return BLAZE_ERROR_RESULT("BlazeEngine", "Cant change the element registry");
		registry = r;

		managers.resize(r.GetElementTypeCount());

		for (uint i = 0; i < r.GetElementTypeCount(); ++i)
		{
			auto data = r.GetElementTypeData(i);
			managers[i] = (ElementManager*)Memory::Allocate(data.managerSize);
			data.constructManager(managers[i]);
		}

		return Result();
	}

}
