#include "App.h"

void Start();

CLIENT_API void Setup()
{
	app = new App;

	Start();

	delete app;
}