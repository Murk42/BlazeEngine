#pragma once

namespace OpenGLWrapper
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
		RGBA,
		RGB,
		RG,
		R,

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

		R11F_G11F_B10F,
	};

	enum class TextureBufferInternalPixelFormat
	{
		R8,	
		R16,		
		R16F,
		R32F,		
		R8I,		
		R16I,		
		R32I,		
		R8UI,		
		R16UI,	
		R32UI,	
		RG8,		
		RG16,		
		RG16F,	
		RG32F,	
		RG8I,		
		RG16I,	
		RG32I,	
		RG8UI,	
		RG16UI,	
		RG32UI,	
		RGB32F,	
		RGB32I,	
		RGB32UI,
		RGBA8,	
		RGBA16,	
		RGBA16F,
		RGBA32F,
		RGBA8I,	
		RGBA16I,
		RGBA32I,
		RGBA8UI,
		RGBA16UI,
		RGBA32UI
	};
}