#pragma once
#include "ElementTypeRegistry.h"

#include "Element.h"
#include "SceneFileParser.h"

namespace Blaze::UI2
{	
	class Manager
	{
		ElementTypeRegistry registry;

		std::vector<ElementManager*> managers;
	public:
		Manager();
		~Manager();

		template<typename T>
		T GetElementManager()
		{
			return (T*)managers[registry.GetElementTypeIndex<T>];
		}
		ElementManager* GetElementManager(uint typeIndex);

		Result SetElementTypeRegistry(const ElementTypeRegistry&);
		const ElementTypeRegistry& GetRegistry() const { return registry; }
	};

	class Scene
	{
		struct DerivedElementContainer
		{
			ElementTypeData typeData;
			size_t size;
			void* ptr;
		};

		Manager* manager;

		std::vector<Element> elements;
		Array<DerivedElementContainer> derivedElements;

		void* CreateDerivedElement(size_t typeIndex);
		void CreateElement(SceneFileParser::Element& data, Result& r);
		
		void UpdateElement(uint& index, Element* parent);

		Mat4f proj2D;
	public:
		Scene();
		~Scene();

		void Clear();

		void CreateElement();
		void SetManager(Manager* manager);

		void Update();
		void Render();

		Result Load(Path path);

		void SetProjectionMatrix(Mat4f proj) { proj2D = proj; }
		Mat4f GetProjectionMatrix() const { return proj2D; }
	};
}