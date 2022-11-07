#pragma once
#include "BlazeEngine/Application/UI System/UISceneBinaryParser.h"
#include "BlazeEngine/Application/UI System/UIScene.h"
#include "BlazeEngine/DataStructures/BinaryOutputStream.h"
#include "BlazeEngine/DataStructures/BinaryInputStream.h"
#include "BlazeEngine/File/File.h"

#include "BlazeEngine/Resources/Font/Font.h"
#include "BlazeEngine/Graphics/GraphicsLibrary.h"

namespace Blaze::UI
{
	void UISceneBinaryParser::Deserialize(UIScene* scene)
	{
		auto* resManager = scene->GetResourceManager();
		auto& resRegistry = resManager->GetResourceTypeRegistry();
		auto& resStorage = scene->GetResourceStorage();
		
		BinaryInputStream stream;
		stream.TakeBuffer(buffer);

		size_t resourceCount;
		stream.Read(resourceCount);

		for (int i = 0; i < resourceCount; ++i)
		{
			String name;
			stream.ReadString(name);

			uint typeIndex;
			stream.Read(typeIndex);
			
			if (typeIndex == resRegistry.GetResourceTypeIndex<Font>())
			{				
				Font* font = resStorage.CreateResource<Font>(name);

				String path;
				stream.ReadString(path);

				font->Load(path);

				uint fontResolutionCount;
				stream.Read(fontResolutionCount);

				for (int j = 0; j < fontResolutionCount; ++j)
				{
					uint resolution;
					stream.Read(resolution);
					uint8 renderType;
					stream.Read(renderType);

					FontResolution* res = font->CreateFontResolution(resolution, (FontResolutionRenderType)renderType);
					res->LoadCharacters(0, 127);
					res->CreateAtlas();
				}
			}
			else if (typeIndex == resRegistry.GetResourceTypeIndex<Graphics::Core::Texture2D>())
			{
				Graphics::Core::Texture2D* tex = resStorage.CreateResource<Graphics::Core::Texture2D>(r.name);

				String path;
				stream.ReadString(path);

				tex->Load(path);

				uint8 min, mag, mip, xWrap, yWrap, mipmaps;
				stream.Read(min);
				stream.Read(mag);
				stream.Read(mip);
				stream.Read(xWrap);
				stream.Read(yWrap);
				stream.Read(mipmaps);

				Graphics::Core::Texture2DSettings settings;
				settings.min = (Graphics::Core::TextureSampling)min;
				settings.mag = (Graphics::Core::TextureSampling)mag;
				settings.mip = (Graphics::Core::TextureSampling)mip;
				settings.xWrap = (Graphics::Core::TextureWrapping)xWrap;
				settings.yWrap = (Graphics::Core::TextureWrapping)yWrap;
				settings.mipmaps = mipmaps;
				
				tex->SetSettings(settings);
			}
		}

		auto* elManager = scene->GetManager();

		size_t elementCount;
		stream.Read(elementCount);

		std::vector<std::pair<UIElement*, uint>> elements;
		elements.resize(elementCount);
		
		for (int i = 0; i < elementCount; ++i)
		{
			uint typeIndex;
			stream.Read(typeIndex);

			uint layer;
			stream.Read(layer);

			String name;
			stream.ReadString(name);

			UIElement* element = scene->CreateElement(name, typeIndex, layer);

			elements[i] = { element, typeIndex };
		}
		for (int i = 0; i < elementCount; ++i)
		{
			uint typeIndex = elements[i].second;
			UIElement* element = elements[i].first;

			auto& elementManager = elManager->GetElementManager(typeIndex);
			elementManager.Deserialize(element, stream);
		}

		stream.ReturnBuffer(buffer);
	}
	void UISceneBinaryParser::LoadFromBuffer(Buffer&& buffer)
	{
		this->buffer = std::move(buffer);
	}
	void UISceneBinaryParser::LoadFromFile(const Path& path)
	{
		File file(path, FileOpenMode::Read);
		buffer.Allocate(file.Size());
		file.Read(buffer);
	}
	void UISceneBinaryParser::Serialize(const UIScene* scene)
	{
		//auto* resManager = scene->GetResourceManager();
		//auto& resRegistry = resManager->GetResourceTypeRegistry();
		//auto& resStorage = scene->GetResourceStorage();
		//size_t resTypeCount = resRegistry.GetResourceTypeCount();
		//
		//BinaryOutputStream stream;
		//
		//
		//size_t resourceCount = 0;
		//for (int i = 0; i < resTypeCount; ++i)
		//	resourceCount += resStorage.GetResourceList(i).size();
		//
		//stream.Write(resourceCount);
		//
		//size_t resTypeIndex = 0;
		//auto* list = &resStorage.GetResourceList(resTypeIndex);
		//auto it = list->begin();
		//for (int i = 0; i < resourceCount; ++i)
		//{
		//	if (it == list->end())
		//	{
		//		++resTypeIndex;
		//		list = &resStorage.GetResourceList(resTypeIndex);
		//		it = list->begin();
		//	}
		//	ResourceSystem::Resource* resource = *it;
		//	++it;
		//	
		//	stream.WriteString(resManager->GetResourceName(resource, resTypeIndex));			
		//	stream.Write(resTypeIndex);
		//
		//	if (resTypeIndex == resRegistry.GetResourceTypeIndex<Font>())
		//	{
		//		Font* font = (Font*)resource;
		//
				//String path;
				//stream.ReadString(path);
				//
				//font->Load(path);
				//
				//uint fontResolutionCount;
				//stream.Read(fontResolutionCount);
				//
				//for (int j = 0; j < fontResolutionCount; ++j)
				//{
				//	uint resolution;
				//	stream.Read(resolution);
				//	uint8 renderType;
				//	stream.Read(renderType);
				//
				//	FontResolution* res = font->CreateFontResolution(resolution, (FontResolutionRenderType)renderType);
				//	res->LoadCharacters(0, 127);
				//	res->CreateAtlas();
				//}
		//	}
		//	else if (resTypeIndex == resRegistry.GetResourceTypeIndex<Graphics::Core::Texture2D>())
		//	{
				//Graphics::Core::Texture2D* tex = resStorage.CreateResource<Graphics::Core::Texture2D>(r.name);
				//
				//String path;
				//stream.ReadString(path);
				//
				//tex->Load(path);
				//
				//uint8 min, mag, mip, xWrap, yWrap, mipmaps;
				//stream.Read(min);
				//stream.Read(mag);
				//stream.Read(mip);
				//stream.Read(xWrap);
				//stream.Read(yWrap);
				//stream.Read(mipmaps);
				//
				//Graphics::Core::Texture2DSettings settings;
				//settings.min = (Graphics::Core::TextureSampling)min;
				//settings.mag = (Graphics::Core::TextureSampling)mag;
				//settings.mip = (Graphics::Core::TextureSampling)mip;
				//settings.xWrap = (Graphics::Core::TextureWrapping)xWrap;
				//settings.yWrap = (Graphics::Core::TextureWrapping)yWrap;
				//settings.mipmaps = mipmaps;
				//
				//tex->SetSettings(settings);
		//	}			
		//}

		//auto* elManager = scene->GetManager();

		//size_t elementCount;
		//stream.Read(elementCount);
		//
		//std::vector<std::pair<UIElement*, uint>> elements;
		//elements.resize(elementCount);
		//
		//for (int i = 0; i < elementCount; ++i)
		//{
		//	uint typeIndex;
		//	stream.Read(typeIndex);
		//
		//	uint layer;
		//	stream.Read(layer);
		//
		//	String name;
		//	stream.ReadString(name);
		//
		//	UIElement* element = scene->CreateElement(name, typeIndex, layer);
		//
		//	elements[i] = { element, typeIndex };
		//}
		//for (int i = 0; i < elementCount; ++i)
		//{
		//	uint typeIndex = elements[i].second;
		//	UIElement* element = elements[i].first;
		//
		//	auto& elementManager = elManager->GetElementManager(typeIndex);
		//	elementManager.Deserialize(element, stream);
		//}
		//
		//stream.ReturnBuffer(buffer);
	}
	void UISceneBinaryParser::SaveToBuffer(Buffer& buffer)
	{
		buffer = this->buffer;
	}
	void UISceneBinaryParser::SaveToFile(const Path& path)
	{
		File file(path, FileOpenMode::Write);
		file.Write(buffer);		
	}
