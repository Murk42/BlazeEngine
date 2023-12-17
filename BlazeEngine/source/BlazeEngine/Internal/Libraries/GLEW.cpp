#include "pch.h"
#include "BlazeEngine/Internal/Libraries/GLEW.h"

//#include "GL/glew.h"
//
//namespace Blaze
//{	
//	TimingResult InitializeCoreGLEW()
//	{
//		Timing timing{ "GLEW initialization" };
//		glewExperimental = GL_TRUE;
//
//		if (glewInit() != GLEW_OK)
//			throw
//			"Failed to initialize the GLEW library!\n"
//			"GLEW error code: " + StringParsing::Convert(glGetError());		
//
//		return timing;
//	}
//
//	TimingResult InitializeGLEW()
//	{
//		Timing timing{ "GLEW" };
//		
//		//timing.AddNode(InitializeCoreGLEW());		
//
//		return timing;
//
//	}
//	void TerminateGLEW()
//	{				
//	}
//}