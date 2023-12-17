#pragma once
#include "OpenGLTextureEnums.h"

namespace Blaze::Graphics::OpenGLWrapper
{
	enum class CubemapFace
	{
		PositiveX = 0x8515,
		NegativeX = 0x8516,
		PositiveY = 0x8517,
		NegativeY = 0x8518,
		PositiveZ = 0x8519,
		NegativeZ = 0x851A,
	};
	enum class CubemapFileType
	{
		VerticalCross,
		HorizontalCross,
		Column,
		Row,
	};

	struct TextureCubemapSettings
	{
		TextureWrapping xWrap = TextureWrapping::ClampToEdge;
		TextureWrapping yWrap = TextureWrapping::ClampToBorder;
		TextureSampling min = TextureSampling::Nearest;
		TextureSampling mag = TextureSampling::Nearest;
		TextureSampling mip = TextureSampling::Nearest;		
		bool mipmaps = false;
	};

	class TextureCubemap
	{
		unsigned id;		
		uint size;
	public:
		TextureCubemap();
		TextureCubemap(const TextureCubemap&) = delete;
		TextureCubemap(TextureCubemap&&) noexcept;
		~TextureCubemap();

		Result SetSettings(TextureCubemapSettings settings);		

		Result Load(StringView path, CubemapFileType type);
		Result Load(StringView path, CubemapFace face);				
		
		uint GetSize() const { return size; }
		inline unsigned GetHandle() const { return id; }

		TextureCubemap& operator=(const TextureCubemap&) = delete;
		TextureCubemap& operator=(TextureCubemap&&) noexcept;
	};
}