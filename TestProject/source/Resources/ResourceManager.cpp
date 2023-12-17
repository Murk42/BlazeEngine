#include "pch.h"
#include "ResourceManager.h"

namespace Resource
{
	void ResourceManager::LoadTexture2DOpenGL(StringView name, Graphics::OpenGLWrapper::Texture2D&& texture)
	{
		texture2DOpenGL.Insert(name, std::move(texture));
	}
	Graphics::OpenGLWrapper::Texture2D ResourceManager::RemoveTexture2DOpenGL(StringView name)
	{
		auto it = texture2DOpenGL.Find(name);
		auto texture = std::move(it->value);
		texture2DOpenGL.Erase(it);
		return texture;
	}
	Graphics::OpenGLWrapper::Texture2D& ResourceManager::GetTexture2DOpenGL(StringView name)
	{
		return texture2DOpenGL.Find(name)->value;
	}
	void ResourceManager::LoadFont(StringView name, Font&& font)
	{ 
		fonts.Insert(name, std::move(font));
	}
	Font ResourceManager::RemoveFont(StringView name)
	{
		auto it = fonts.Find(name);
		auto font = std::move(it->value);
		fonts.Erase(it);
		return font;
	}
	Font& ResourceManager::GetFont(StringView name)
	{
		return fonts.Find(name)->value;
	}	
}