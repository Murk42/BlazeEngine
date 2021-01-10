#pragma once
#include "BlazeEngine/Core/EngineCore.h"

int BLAZE_API main();

namespace Blaze
{				
	class BLAZE_API BaseApplication
	{			
		using Constructor = void(*)(BaseApplication*);
		using Destructor = void(*)(BaseApplication*);
		static void Initialize(Constructor, Destructor, size_t);
	public:		
		virtual void Startup() { }
		virtual void Cleanup() { }
		virtual void Frame() { }

		static void Stop();			
		static BaseApplication& Instance();				
		
		template<typename>
		friend class Application;
		friend void InitializeBlaze();
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
				Initialize(Construct, Destruct, sizeof(Derived));
			}			
		} inline static init;
	public:			
		static Derived& Instance() { return *(Derived*)&BaseApplication::Instance(); }
	};	
}
