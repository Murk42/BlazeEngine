#pragma once
#include "OpenGLTextureEnums.h"

namespace Blaze::Graphics::OpenGLWrapper
{	
	struct Texture3DSettings
	{
		TextureWrapping xWrap = TextureWrapping::ClampToEdge;
		TextureWrapping yWrap = TextureWrapping::ClampToBorder;
		TextureSampling min = TextureSampling::Nearest;
		TextureSampling mag = TextureSampling::Nearest;
		TextureSampling mip = TextureSampling::Nearest;
		bool mipmaps = false;
	};

	class Texture3D
	{
		unsigned id;
		Vec3u size;					
	public:
		Texture3D();
		Texture3D(const Texture3D&) = delete;
		Texture3D(Texture3D&&) noexcept;
		~Texture3D();

		Result SetSettings(Texture3DSettings settings);				
						
		Result Create(Vec3u size, TextureInternalPixelFormat internalFormat);				
		Result SetPixels(Vec3u offset, Vec3u size, uint stride, BitmapColorFormat format, BitmapColorComponentType type, void* pixels);						

		Vec3u GetSize() const { return size; }				

		inline unsigned GetHandle() const { return id; }

		Texture3D& operator=(const Texture3D&) = delete;
		Texture3D& operator=(Texture3D&&) noexcept;
	};
}