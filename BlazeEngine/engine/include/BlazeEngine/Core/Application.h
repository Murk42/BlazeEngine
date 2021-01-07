#pragma once
#include "BlazeEngine/Core/EngineCore.h"

int BLAZE_API main();

namespace Blaze
{			
	class BLAZE_API BaseApplication
	{			
		static void Setup(void(*a)(BaseApplication*), void(*b)(BaseApplication*), size_t);		
	public:		
		virtual void Startup() { }
		virtual void Cleanup() { }
		virtual void Frame() { }

		static void Stop();			
		static BaseApplication* Instance();				
		
		template<typename>
		friend class Application;
	};

	template<typename Derived>
	class Application : public BaseApplication
	{		
		static void Construct(BaseApplication* app) { new (app) Derived(); }
		static void Destruct(BaseApplication* app) { ((Derived*)app)->~Derived(); }		

		struct Init
		{
			Init()
			{
				BaseApplication::Setup(Construct, Destruct, sizeof(Derived));
			}			
		} inline static init;
	public:			
		static Derived& Instance() { return *(Derived*)BaseApplication::Instance(); }
	};	
}
