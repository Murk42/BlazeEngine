#pragma once
#include "BlazeEngine/Resources/Bitmap/Bitmap.h"
#include "BlazeEngine/Graphics/OpenGL/Textures/OpenGLTextureEnums.h"

namespace Blaze::OpenGL
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

	class BLAZE_API Texture3D
	{
		unsigned id;
		Vec3i size;					
	public:
		Texture3D();
		Texture3D(const Texture3D&) = delete;
		Texture3D(Texture3D&&) noexcept;
		~Texture3D();

		void SetSettings(Texture3DSettings settings);				
						
		void Create(Vec3i size, TextureInternalPixelFormat internalFormat);				
		void SetPixels(Vec3i offset, Vec3i size, uint stride, BitmapPixelFormat format, BitmapPixelType type, void* pixels);						

		Vec3i GetSize() const { return size; }				

		inline unsigned GetHandle() const { return id; }

		Texture3D& operator=(const Texture3D&) = delete;
		Texture3D& operator=(Texture3D&&) noexcept;
	};
}