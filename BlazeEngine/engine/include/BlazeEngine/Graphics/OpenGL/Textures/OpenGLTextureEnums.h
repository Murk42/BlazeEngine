#pragma once

namespace Blaze::OpenGL
{
	enum class TextureWrapping
	{
		ClampToBorder = 0x812D,
		ClampToEdge = 0x812F,
		MirroredRepeat = 0x8370,
		Repeat = 0x2901,
	};

	enum class TextureSampling
	{
		Nearest = 0x2600,
		Linear = 0x2601,
	};

	enum class TextureInternalPixelFormat
	{
		RGBA8,
		RGB8,
		RG8,
		R8,
							
		RGBA16,
		RGB16,
		RG16,
		R16,
							
		RGBA16F,
		RGB16F,
		RG16F,
		R16F,
							
		RGBA32F,
		RGB32F,
		RG32F,
		R32F,
							
		RGBA8I,
		RGB8I,
		RG8I,
		R8I,
							
		RGBA16I,
		RGB16I,
		RG16I,
		R16I,
							
		RGBA32I,
		RGB32I,
		RG32I,
		R32I,
							
		RGBA8UI,
		RGB8UI,
		RG8UI,
		R8UI,
							
		RGBA16UI,
		RGB16UI,
		RG16UI,
		R16UI,
							
		RGBA32UI,
		RGB32UI,
		RG32UI,
		R32UI,
							
		Depth16,
		Depth24,
		Depth32F,
							
		Depth24Stencil8,
		Depth32FStencil8,							
		Stencil8,
	};
}