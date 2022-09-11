#pragma once
#include "BlazeEngine/Resources/Bitmap/Bitmap.h"
#include "BlazeEngine/Graphics/OpenGL/Textures/OpenGLTextureEnums.h"

namespace Blaze::OpenGL
{	
	struct Texture1DSettings
	{
		TextureWrapping xWrap = TextureWrapping::ClampToEdge;		
		TextureSampling min = TextureSampling::Nearest;
		TextureSampling mag = TextureSampling::Nearest;
		TextureSampling mip = TextureSampling::Nearest;
		bool mipmaps = false;
	};

	class BLAZE_API Texture1D
	{
		unsigned id;
		size_t size;
	public:
		Texture1D();
		Texture1D(const Texture1D&) = delete;
		Texture1D(Texture1D&&) noexcept;	
		~Texture1D();

		void SetSettings(Texture1DSettings settings);		

		bool Load(StringView path, bool emitLogOnFail = true);		
		bool Load(StringView path, TextureInternalPixelFormat internalFormat, bool emitLogOnFail = true);
						
		void Create(size_t size, TextureInternalPixelFormat internalFormat);			
		void SetPixels(size_t offset, size_t size, BitmapPixelFormat format, BitmapPixelType type, void* pixels);				

		void GenerateMipmaps();

		size_t GetSize() const { return size; }		

		inline unsigned GetHandle() const { return id; }

		Texture1D& operator=(const Texture1D&) = delete;
		Texture1D& operator=(Texture1D&&) noexcept;
	};
}