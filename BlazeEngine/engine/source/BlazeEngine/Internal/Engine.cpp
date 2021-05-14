#include "source/BlazeEngine/Internal/Engine.h"

namespace Blaze
{	
	Engine* engine = nullptr; 

	void Terminate();

	Engine::Engine()
	{
		Renderer.boundVertexArray = &Renderer.defaultVertexArray;
	}
	Engine::~Engine()
	{
		Terminate();
	} 
}