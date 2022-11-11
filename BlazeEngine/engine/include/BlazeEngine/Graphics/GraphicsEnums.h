#pragma once

namespace Blaze
{	
	enum class DisplayPixelFormat
	{
		Any,
		RGB332,
		RGB444,
		RGB555,
		BGR555,
		ARGB4444,
		RGBA4444,
		ABGR4444,
		BGRA4444,
		ARGB1555,
		RGBA5551,
		ABGR1555,
		BGRA5551,
		RGB565,
		BGR565,
		RGB24,
		BGR24,
		RGB888,
		RGBX8888,
		BGR888,
		BGRX8888,
		ARGB8888,
		RGBA8888,
		ABGR8888,
		BGRA8888,
		ARGB2101010
	};

	namespace Graphics::Core
	{
		enum class IndexType
		{
			Uint32,
			Uint16,
			Uint8,
		};
		enum class PolygonMode
		{
			Point,
			Line,
			Fill
		};
		enum class PrimitiveType
		{
			Points,
			Lines,
			LineStrip,
			LineLoop,
			Triangles,
			TriangleStrip,
			TriangleFan,
			Pathes
		};
		enum class ImageAccess
		{
			Read,
			Write,
			ReadWrite,
		};
		enum class ImageFormat
		{
			RGBA32F			,
			RGBA16F			,
			RG32F			,
			RG16F			,
			R11F_G11F_B10F	,
			R32F			,
			R16F			,
			RGBA32UI		,
			RGBA16UI		,
			RGB10_A2UI		,
			RGBA8UI			,
			RG32UI			,
			RG16UI			,
			RG8UI			,
			R32UI			,
			R16UI			,
			R8UI			,
			RGBA32I			,
			RGBA16I			,
			RGBA8I			,
			RG32I			,
			RG16I			,
			RG8I			,
			R32I			,
			R16I			,
			R8I				,
			RGBA16			,
			RGB10_A2		,
			RGBA8			,
			RG16			,
			RG8				,
			R16				,
			R8				,
			RGBA16_SNORM	,
			RGBA8_SNORM		,
			RG16_SNORM		,
			RG8_SNORM		,
			R16_SNORM		,
			R8_SNORM		,			
		};		
	}
}