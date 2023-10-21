#pragma once
#include "OpenGLTextureEnums.h"

namespace OpenGLWrapper
{
	struct Texture2DArraySettings
	{
		TextureWrapping xWrap = TextureWrapping::ClampToEdge;
		TextureWrapping yWrap = TextureWrapping::ClampToBorder;
		TextureSampling min = TextureSampling::Nearest;
		TextureSampling mag = TextureSampling::Nearest;
		TextureSampling mip = TextureSampling::Nearest;
		bool mipmaps = false;
	};

	class Texture2DArray
	{
		unsigned id;		
		Vec2i size;
		uint layers;
	public:
		Texture2DArray(); 
		Texture2DArray(const Texture2DArray&) = delete;
		Texture2DArray(Texture2DArray&&) noexcept;				
		~Texture2DArray();

		Result SetSettings(Texture2DArraySettings settings);		

		Result Create(Vec2i size, uint layers, TextureInternalPixelFormat internalFormat);
		Result SetPixels(Vec2i offset, uint layer, BitmapView bm);

		inline Vec2i GetSize() const { return size; }
		inline size_t GetDepth() const { return layers; }
		inline unsigned GetHandle() const { return id; }

		Texture2DArray& operator=(const Texture2DArray&) = delete;
		Texture2DArray& operator=(Texture2DArray&&) noexcept;
	};
}