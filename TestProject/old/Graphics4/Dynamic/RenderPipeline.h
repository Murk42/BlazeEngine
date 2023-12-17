#pragma once
#include "Graphics4/Dynamic/Core/GraphicsDynamicImplementation.h"

namespace Graphics4::Dynamic
{	
	class RenderWindow_Dynamic;

	//Implemented in an outside class
	class UIRenderTask_OpenGL
	{
	public:
		void ExecuteTask(/* ... */);
	};
		
	//CO class
	class InGameRenderPipeline_OpenGL
	{
	public:
		InGameRenderPipeline_OpenGL(
			RenderWindow& renderWindow,
			UIRenderTask_OpenGL& uiRenderTask
		);

		void ExecutePipeline();
	};

	//CO class
	class InGameRenderPipeline_Vulkan
	{
	public:
		InGameRenderPipeline_Vulkan(
			RenderWindow& renderWindow,
			UIRenderTask_OpenGL& uiRenderTask
		);

		void ExecutePipeline();
	};		
}