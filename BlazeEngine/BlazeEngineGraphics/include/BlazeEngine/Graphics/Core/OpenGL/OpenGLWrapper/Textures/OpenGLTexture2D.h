#pragma once
#include "BlazeEngine/Common/Bitmap.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"

namespace Blaze::Graphics::OpenGL
{
	struct Texture2DSettings
	{
		TextureWrapping xWrap = TextureWrapping::ClampToEdge;
		TextureWrapping yWrap = TextureWrapping::ClampToEdge;
		TextureSampling min = TextureSampling::Nearest;
		TextureSampling mag = TextureSampling::Nearest;
		TextureSampling mip = TextureSampling::Nearest;
		uint textureLevelCount = 1;
	};

	class Texture2D
	{
	public:
		Texture2D();
		Texture2D(const Texture2D&) = delete;
		Texture2D(Texture2D&&) noexcept;
		Texture2D(Vec2u size, TextureInternalPixelFormat internalFormat, const Texture2DSettings& settings = {});
		Texture2D(BitmapView bm, const Texture2DSettings& settings = {});
		Texture2D(BitmapView bm, TextureInternalPixelFormat internalFormat, const Texture2DSettings& settings = {});
		Texture2D(Path path, const Texture2DSettings& settings = {});
		Texture2D(Path path, TextureInternalPixelFormat internalFormat, const Texture2DSettings& settings = {});
		~Texture2D();

		void SetSwizzle(TextureSwizzle red, TextureSwizzle green, TextureSwizzle blue, TextureSwizzle alpha);

		void Create(Vec2u size, TextureInternalPixelFormat internalFormat, const Texture2DSettings& settings = {});
		void Create(BitmapView bm, const Texture2DSettings& settings = {});
		void Create(BitmapView bm, TextureInternalPixelFormat internalFormat, const Texture2DSettings& settings = {});

		void Load(Path path, const Texture2DSettings& settings = {});
		void Load(Path path, TextureInternalPixelFormat internalFormat, const Texture2DSettings& settings = {});

		void SetPixels(Vec2u offset, BitmapView bm, uint textureLevel);

		Bitmap GetBitmap(BitmapColorFormat colorFormat, BitmapColorComponentType componentType) const;

		void AutoGenerateMipmaps();

		Vec2u GetSize() const { return size; }

		inline unsigned GetHandle() const { return id; }

		Texture2D& operator=(const Texture2D&) = delete;
		Texture2D& operator=(Texture2D&&) noexcept;
	private:
		void SetSettings(const Texture2DSettings& settings);
		void RecreateIfNeeded();

		unsigned id;
		Vec2u size;
	};
}