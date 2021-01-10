#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Graphics/Bitmap.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/DataStructures/Vector.h"

namespace Blaze
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
	
	struct TextureSettings
	{
		TextureWrapping xWrap = TextureWrapping::ClampToEdge;
		TextureWrapping yWrap = TextureWrapping::ClampToEdge;
		TextureSampling min = TextureSampling::Linear;
		TextureSampling mag = TextureSampling::Linear;
		TextureSampling mip = TextureSampling::Linear;
	};	

	enum class TextureType
	{
		Single1D = 0x0DE0,
		Single2D = 0x0DE1,
		Single3D = 0x806F,
		Cubemap = 0x8513,
		Rectangle = 0x84F5,
		Array1D = 0x8C18,
		Array2D = 0x8C1A,
	};

	class BLAZE_API Texture
	{				
		unsigned id;				
		TextureSettings settings;
		TextureType type;
		bool hasMipmaps;
		
		Texture(TextureType type);
		
	public:		
		~Texture();

		TextureType GetType() const;						
		TextureSettings GetSettings() const;				

		void SetSettings(TextureSettings settings);
		void SetSettings(TextureWrapping xWrap, TextureWrapping yWrap);
		void SetSettings(TextureSampling min, TextureSampling mag);
		void SetSettings(TextureWrapping xWrap, TextureWrapping yWrap, TextureSampling min, TextureSampling mag);
		void SetSettings(TextureSampling min, TextureSampling mag, TextureSampling mipmapSampling);		
		void SetSettings(TextureWrapping xWrap, TextureWrapping yWrap, TextureSampling min, TextureSampling mag, TextureSampling mipmapSampling);

		void GenerateMipmaps();

		void Bind() const;
		void Bind(unsigned slot) const;		

		Texture& operator=(Texture&&) noexcept;

		template<TextureType>
		class Base;
		friend class Texture1D;
		friend class Texture2D;
		friend class Texture3D;
		friend class TextureCubemap;
		friend class TextureRectangle;
		friend class TextureArray1D;
		friend class TextureArray2D;
	};

	class BLAZE_API Texture2D : public Texture
	{		
		Vec2i size;
		PixelFormat format;
	public:
		Texture2D();
		Texture2D(const String& path);

		void Load(const String& path);
		void Save(const String& path);	

		void Create(const Bitmap& bm);
		void Create(Vec2i size, PixelFormat format, void* pixels);

		void SetPixels(Vec2i offset, Vec2i size, PixelFormat format, void* pixels);
		void GetPixels(Bitmap& bm);		
		
		Vec2i GetSize() const { return size; }
		PixelFormat GetFormat() const { return format; }
		
		Texture2D& operator=(Texture2D&&) noexcept;
	};

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
	class BLAZE_API TextureCubemap : public Texture
	{		
		PixelFormat format;
		uint size;
	public:
		TextureCubemap();
		TextureCubemap(const String& xp, const String& xn, const String& yp, const String& yn, const String& zp, const String& zn);
		
		void Load(StringView path, CubemapFileType type);
		void Load(const String& path, CubemapFace face);
		void Load(const String& xp, const String& xn, const String& yp, const String& yn, const String& zp, const String& zn);
		void Save(const String& path, CubemapFace face);

		void GetPixels(Bitmap& bm, CubemapFace face);


		PixelFormat GetFormat() const { return format; }
		uint GetSize() const { return size; }

	};

	class BLAZE_API TextureArray2D : public Texture
	{
		PixelFormat format;
		Vec3i size;
	public:
		TextureArray2D();
		TextureArray2D(StringView path, Vec2i textureSize);

		void Load(StringView path, Vec2i textureSize);
		void Save(StringView path);

		void GetPixels(Bitmap& bm);

		Vec3i GetSize() const { return size; }
	};
}