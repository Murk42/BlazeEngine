#pragma once
#include "BlazeEngine/Core/EngineCore.h"

int BLAZE_API main();

namespace Blaze
{				
	class Log;
	class Window;

	namespace Application
	{		
		BLAZE_API void SetTargetFramerate(uint fps);
		BLAZE_API void SetTargetDeltaTime(double deltaTime);

		BLAZE_API void Stop();

		BLAZE_API uint GetTargetDeltaTime();
		BLAZE_API uint GetFPS();
		BLAZE_API double GetDeltaTime();
	}

	class BLAZE_API BlazeEngineBase
	{			
		using Constructor = void(*)(BlazeEngineBase*);
		using Destructor = void(*)(BlazeEngineBase*);		
		static void Initialize(Constructor, Destructor, size_t);
	public:

		virtual void Startup() { }
		virtual void Cleanup() { }
		virtual void Frame() { }
		
		virtual void NewLog(const Log& log);			
		
		template<typename>
		friend class BlazeEngine;
		friend void InitializeBlaze();		
	};	

	template<typename Derived>
	class BlazeEngine : protected BlazeEngineBase
	{		
		static void Construct(BlazeEngineBase* app) { new (app) Derived(); }
		static void Destruct(BlazeEngineBase* app) { ((Derived*)app)->~Derived(); }		

		struct Init
		{			
			Init()				 
			{		
				Initialize(Construct, Destruct, sizeof(Derived));
			}			
		} inline static init;			
	};	
}
