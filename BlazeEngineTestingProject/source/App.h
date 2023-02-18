#pragma once

class App
{
	ResourceSystem::ResourceManager resourceManager;
	ResourceSystem::ResourceStorage resourceStorage;		

	ECS::Manager ecsManager;
};

extern App* app;