#pragma once

namespace Blaze::ECS
{
	class NameReferenceComponentSystem;

	class NameReferenceComponent : public Component
	{
		String name;
	public:
		void SetName(String name);

		inline String GetName() const { return name; }

		COMPONENT(NameReferenceComponent, NameReferenceComponentSystem)
	};

	class NameReferenceComponentSystem : public System
	{
		std::unordered_map<String, NameReferenceComponent*> map;
	public:
		NameReferenceComponent* GetComponent(StringView name) const;

		bool SetProperty(Component*, StringView, StringView) override;

		friend class NameReferenceComponent;
	};
}