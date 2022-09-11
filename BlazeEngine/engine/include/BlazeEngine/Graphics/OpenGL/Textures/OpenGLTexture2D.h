#pragma once
#include "BlazeEngine/Resources/Bitmap/Bitmap.h"
#include "BlazeEngine/Graphics/OpenGL/Textures/OpenGLTextureEnums.h"

namespace Blaze::OpenGL
{	
	struct Texture2DSettings
	{
		TextureWrapping xWrap = TextureWrapping::ClampToEdge;
		TextureWrapping yWrap = TextureWrapping::ClampToBorder;
		TextureSampling min = TextureSampling::Nearest;
		TextureSampling mag = TextureSampling::Nearest;
		TextureSampling mip = TextureSampling::Nearest;
		bool mipmaps = false;
	};

	class BLAZE_API Texture2D
	{
		unsigned id;
		Vec2i size;			
	public:
		Texture2D();
		Texture2D(const Texture2D&) = delete;
		Texture2D(Texture2D&&) noexcept;	
		~Texture2D();

		void SetSettings(Texture2DSettings settings);		

		bool Load(StringView path, bool emitLogOnFail = true);		
		bool Load(StringView path, TextureInternalPixelFormat internalFormat, bool emitLogOnFail = true);
						
		void Create(Vec2i size, TextureInternalPixelFormat internalFormat);
		void Create(BitmapView bm);								
		void Create(BitmapView bm, TextureInternalPixelFormat internalFormat);
		void SetPixels(Vec2i offset, BitmapView bm);
		void SetPixels(Vec2i offset, Vec2i size, uint stride, BitmapPixelFormat format, BitmapPixelType type, void* pixels);				

		void GenerateMipmaps();

		Vec2i GetSize() const { return size; }		

		inline unsigned GetHandle() const { return id; }

		Texture2D& operator=(const Texture2D&) = delete;
		Texture2D& operator=(Texture2D&&) noexcept;

		static constexpr const char* typeName = "Texture2D";
	};
}