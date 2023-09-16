#pragma once
#include "BlazeEngine/Application/ECS/System.h"

namespace Blaze::UI
{
	class UIScene;	
	class UIComponent;	

	class BLAZE_API UISystem : public ECS::System
	{		
	public:
		UISystem();
	
		virtual void PreTransform(UIScene* scene, UIComponent*) { }
		virtual void PostTransform(UIScene* scene, UIComponent*) { }
		virtual void Render(UIScene* scene, UIComponent*) { }

		virtual bool PreUpdate(UIScene* scene) { return true; }
		virtual void Update(UIScene* scene, UIComponent* component) { }
		virtual void PostUpdate(UIScene* scene) { }		

		friend class UIScene;
	};
}