#pragma once
#include "BlazeEngine/Common/Bitmap.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"

namespace Blaze::Graphics::OpenGL
{
	struct Texture1DSettings
	{
		TextureWrapping xWrap = TextureWrapping::ClampToEdge;
		TextureSampling min = TextureSampling::Nearest;
		TextureSampling mag = TextureSampling::Nearest;
		TextureSampling mip = TextureSampling::Nearest;
		uint textureLevelCount = 1;
	};

	class Texture1D
	{
	public:
		Texture1D();
		Texture1D(const Texture1D&) = delete;
		Texture1D(Texture1D&&) noexcept;
		~Texture1D();

		void Create(uint size, TextureInternalPixelFormat internalFormat, const Texture1DSettings& settings);
		void Create(uint size, TextureInternalPixelFormat internalFormat, BitmapColorFormat format, BitmapColorComponentType type, void* pixels, const Texture1DSettings& settings);
		void SetPixels(uint offset, uint size, BitmapColorFormat format, BitmapColorComponentType type, void* pixels, uint textureLevel);

		void AutoGenerateMipmaps();

		uint GetSize() const { return size; }

		inline unsigned GetHandle() const { return id; }

		Texture1D& operator=(const Texture1D&) = delete;
		Texture1D& operator=(Texture1D&&) noexcept;
	private:
		void SetSettings(const Texture1DSettings& settings);

		unsigned id;
		uint size;
	};
}