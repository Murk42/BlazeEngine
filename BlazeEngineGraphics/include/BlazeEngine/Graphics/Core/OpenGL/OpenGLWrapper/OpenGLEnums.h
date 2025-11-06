#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"

namespace Blaze::Graphics::OpenGL
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
		RGBA32F,
		RGBA16F,
		RG32F,
		RG16F,
		R11F_G11F_B10F,
		R32F,
		R16F,
		RGBA32UI,
		RGBA16UI,
		RGB10_A2UI,
		RGBA8UI,
		RG32UI,
		RG16UI,
		RG8UI,
		R32UI,
		R16UI,
		R8UI,
		RGBA32I,
		RGBA16I,
		RGBA8I,
		RG32I,
		RG16I,
		RG8I,
		R32I,
		R16I,
		R8I,
		RGBA16,
		RGB10_A2,
		RGBA8,
		RG16,
		RG8,
		R16,
		R8,
		RGBA16_SNORM,
		RGBA8_SNORM,
		RG16_SNORM,
		RG8_SNORM,
		R16_SNORM,
		R8_SNORM,
	};
	enum class StencilOperationType
	{
		Keep,
		Zero,
		Replace,
		Increase,
		IncreaseWrap,
		Decrease,
		DecreaseWrap,
		Invert
	};
	enum class StencilComparison
	{
		Never,
		Less,
		LessEqual,
		Greater,
		GreaterEqual,
		Equal,
		NotEqual,
		Always
	};
	enum class ScreenBufferType
	{
		Front,
		Back,
		BackAndFront,
	};
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
	enum class TextureSwizzle
	{
		R, G, B, A, One, Zero
	};
	enum class IntegerVertexAttributeType
	{
		Int8,
		Int16,
		Int32,
		Uint8,
		Uint16,
		Uint32,		
	};
	enum class FloatVertexAttributeType
	{
		Int8,
		Int16,
		Int32,
		Uint8,
		Uint16,
		Uint32,
		Half,
		Float,
		Double,
	};
	enum class PackedVertexAttributeType
	{
		Int_2_10_10_10_Rev,
		Uint_2_10_10_10_Rev,
		Uint_10F_11F_11F_Rev
	};
	enum class BGRAVertexAttributeType
	{
		Uint8,
		Int_2_10_10_10_Rev,
		Uint_2_10_10_10_Rev
	};
	enum class FramebufferAttachment
	{
		DepthStencil,
		Stencil,
		Depth,
	};
	enum class FramebufferStatus
	{
		Complete,
		Undefined,
		IncompleteAttachment,
		MissingAttachment,
		IncompleteDrawBuffer,
		IncompleteReadBuffer,
		Unsupported,
		IncompleteMultisample,
		IncompleteLayerTargets
	};	
	enum class ShaderType
	{
		FragmentShader = 35632,
		VertexShader = 35633,
		GeometryShader = 36313,
		TessellationControlShader = 36488,
		TessellationEvaluationShader = 36487,
		ComputeShader = 37305,
	};	
	enum class UniformType
	{
		Float,
		Vec2f,
		Vec3f,
		Vec4f,
		Double,
		Vec2d,
		Vec3d,
		Vec4d,
		Int,
		Vec2i,
		Vec3i,
		Vec4i,
		UnsignedInt,
		Vec2u,
		Vec3u,
		Vec4u,
		Bool,
		BoolVec2,
		BoolVec3,
		BoolVec4,
		Mat2f,
		Mat3f,
		Mat4f,
		Mat2x3f,
		Mat2x4f,
		Mat3x2f,
		Mat3x4f,
		Mat4x2f,
		Mat4x3f,
		Mat2d,
		Mat3d,
		Mat4d,
		Mat2x3d,
		Mat2x4d,
		Mat3x2d,
		Mat3x4d,
		Mat4x2d,
		Mat4x3d,
		Sampler1D,
		Sampler2D,
		Sampler3D,
		SamplerCube,
		Sampler1DShadow,
		Sampler2DShadow,
		Sampler1DArray,
		Sampler2DArray,
		Sampler1DArrayShadow,
		Sampler2DArrayShadow,
		Sampler2DMultisample,
		Sampler2DMultisampleArray,
		SamplerCubeShadow,
		SamplerBuffer,
		Sampler2DRect,
		Sampler2DRectShadow,
		IntSampler1D,
		IntSampler2D,
		IntSampler3D,
		IntSamplerCube,
		IntSampler1DArray,
		IntSampler2DArray,
		IntSampler2DMultisample,
		IntSampler2DMultisampleArray,
		IntSamplerBuffer,
		IntSampler2DRect,
		UnsignedIntSampler1D,
		UnsignedIntSampler2D,
		UnsignedIntSampler3D,
		UnsignedIntSamplerCube,
		UnsignedIntSampler1DArray,
		UnsignedIntSampler2DArray,
		UnsignedIntSampler2DMultisample,
		UnsignedIntSampler2DMultisampleArray,
		UnsignedIntSamplerBuffer,
		UnsignedIntSampler2DRect,
		Image1D,
		Image2D,
		Image3D,
		Image2DRect,
		ImageCube,
		ImageBuffer,
		Image1DArray,
		Image2DArray,
		Image2DMultisample,
		Image2DMultisampleArray,
		IntImage1D,
		IntImage2D,
		IntImage3D,
		IntImage2DRect,
		IntImageCube,
		IntImageBuffer,
		IntImage1DArray,
		IntImage2DArray,
		IntImage2DMultisample,
		IntImage2DMultisampleArray,
		UnsignedIntImage1D,
		UnsignedIntImage2D,
		UnsignedIntImage3D,
		UnsignedIntImage2DRect,
		UnsignedIntImageCube,
		UnsignedIntImageBuffer,
		UnsignedIntImage1DArray,
		UnsignedIntImage2DArray,
		UnsignedIntImage2DMultisample,
		UnsignedIntImage2DMultisampleArray,
		UnsignedIntAtomicCounter,
		Invalid
	};
	enum class ShaderProgramState
	{
		Invalid,
		UnsuccesfullyLinked,
		SuccesfullyLinked,
	};
	enum class FenceReturnState
	{
		AlreadySignaled,
		TimeoutExpired,
		ConditionSatisfied,
		Error,
		FenceNotSet,
	};	
	enum class GraphicsBufferMapAccessFlags
	{
		Read = 1,
		Write = 2,
	};
	ENUM_CLASS_BITWISE_OPERATIONS(GraphicsBufferMapAccessFlags);
	enum class GraphicsBufferMapType
	{
		None = 0,
		PersistentUncoherent = 1,
		PersistentCoherent = 2,
	};
	ENUM_CLASS_BITWISE_OPERATIONS(GraphicsBufferMapType);
	enum class GraphicsBufferMapOptions
	{
		None = 0,
		InvalidateRange = 1,
		InvalidateBuffer = 2,
		ExplicitFlush = 4,
		Unsynchronized = 8
	};
	ENUM_CLASS_BITWISE_OPERATIONS(GraphicsBufferMapOptions);
	enum class GraphicsBufferUseFrequency
	{
		Static,
		Dynamic,
		Stream
	};

}