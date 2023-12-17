#pragma once
#include "BlazeEngineGraphics/BlazeEngineGraphics.h"

namespace Resource
{				
	class ResourceManager
	{
		Map<String, Graphics::OpenGLWrapper::Texture2D> texture2DOpenGL;
		Map<String, Font> fonts;		
	public:
		void LoadTexture2DOpenGL(StringView name, Graphics::OpenGLWrapper::Texture2D&& texture);
		Graphics::OpenGLWrapper::Texture2D RemoveTexture2DOpenGL(StringView name);
		Graphics::OpenGLWrapper::Texture2D& GetTexture2DOpenGL(StringView name);

		void LoadFont(StringView name, Font&& font);
		Font RemoveFont(StringView name);
		Font& GetFont(StringView name);		
	};
}