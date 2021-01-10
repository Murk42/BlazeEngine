#include "Engine.h"

namespace Blaze
{	
	Engine* engine = nullptr;

	void Terminate();

	Engine::Engine()
	{
		Renderer.boundVertexLayout = &Renderer.defaultVAO;
	}
	Engine::~Engine()
	{
		Terminate();
	} 
}