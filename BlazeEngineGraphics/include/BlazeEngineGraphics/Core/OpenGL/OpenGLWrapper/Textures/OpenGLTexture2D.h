#pragma once
#include "OpenGLTextureEnums.h"

namespace Blaze::Graphics::OpenGLWrapper
{	
	struct Texture2DSettings
	{
		TextureWrapping xWrap = TextureWrapping::ClampToEdge;
		TextureWrapping yWrap = TextureWrapping::ClampToEdge;
		TextureSampling min = TextureSampling::Nearest;
		TextureSampling mag = TextureSampling::Nearest;
		TextureSampling mip = TextureSampling::Nearest;
		bool mipmaps = false;
	};

	class Texture2D
	{
		unsigned id;
		Vec2u size;					
	public:
		Texture2D();
		Texture2D(const Texture2D&) = delete;
		Texture2D(Texture2D&&) noexcept;	
		~Texture2D();

		Result SetSettings(Texture2DSettings settings);		
		void SetSwizzle(TextureSwizzle red, TextureSwizzle green, TextureSwizzle blue, TextureSwizzle alpha);

		Result Load(Path path);		
		Result Load(Path path, TextureInternalPixelFormat internalFormat);
						
		Result Create(Vec2u size, TextureInternalPixelFormat internalFormat);
		Result Create(BitmapView bm);								
		Result Create(BitmapView bm, TextureInternalPixelFormat internalFormat);

		Result CopyPixels(Vec2u offset, BitmapView bm);		

		Bitmap GetBitmap(BitmapColorFormat colorFormat, BitmapColorComponentType componentType) const;

		void GenerateMipmaps();

		Vec2u GetSize() const { return size; }				

		inline unsigned GetHandle() const { return id; }

		Texture2D& operator=(const Texture2D&) = delete;
		Texture2D& operator=(Texture2D&&) noexcept;		
	};
}