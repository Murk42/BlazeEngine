#pragma once

namespace Blaze::Graphics::OpenGLWrapper
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
	enum class VertexAttributeType
	{
		Int8,
		Uint8,
		Int16,
		Uint16,
		Int32,
		Uint32,
		Float,
		Double
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
	enum class ImmutableGraphicsBufferMapAccess
	{
		Read = 1,
		Write = 2,
	};
	enum class ImmutableGraphicsBufferMapType
	{
		None = 0,
		PersistentUncoherent = 1,
		PersistentCoherent = 2,
	};
	enum class ImmutableGraphicsBufferMapOptions
	{
		None = 0,
		InvalidateRange = 1,
		InvalidateBuffer = 2,
		ExplicitFlush = 4,
		Unsynchronized = 8
	};
	enum class MutableGraphicsBufferUseFrequency
	{
		Static,
		Dynamic,
		Stream
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
	enum class ShaderState
	{
		Invalid,
		UnsuccesfullyCompiled,
		SuccesfullyCompiled,
	};
	enum class UniformType
	{
		Mat4 = 35676,
		Mat3 = 35675,
		Mat2 = 35674,
		Vec4 = 35666,
		Vec3 = 35665,
		Vec2 = 35664,
		Sampler2D = 35678,
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

	ENUM_CLASS_BITWISE_OPERATIONS(ImmutableGraphicsBufferMapAccess);
	ENUM_CLASS_BITWISE_OPERATIONS(ImmutableGraphicsBufferMapType);
	ENUM_CLASS_BITWISE_OPERATIONS(ImmutableGraphicsBufferMapOptions);
}