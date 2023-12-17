#pragma once
#include "BlazeEngineCore/Resources/Bitmap/Bitmap.h"
#include "BlazeEngineCore/Graphics/OpenGL/Textures/OpenGLTextureEnums.h"
#include "BlazeEngineCore/Application/ResourceSystem/Resource.h"
#include "BlazeEngineCore/File/Path.h"

namespace Blaze::OpenGL
{	
	struct Texture2DSettings
	{
		TextureWrapping xWrap = TextureWrapping::ClampToEdge;
		TextureWrapping yWrap = TextureWrapping::ClampToEdge;
		TextureSampling min = TextureSampling::Nearest;
		TextureSampling mag = TextureSampling::Nearest;
		TextureSampling mip = TextureSampling::Nearest;
		bool mipmaps = false;
	};

	class BLAZE_CORE_API Texture2D : public ResourceSystem::Resource
	{
		unsigned id;
		Vec2i size;			
		TextureInternalPixelFormat internalFormat;
	public:
		Texture2D();
		Texture2D(const Texture2D&) = delete;
		Texture2D(Texture2D&&) noexcept;	
		~Texture2D();

		Result SetSettings(Texture2DSettings settings);		

		Result Load(Path path) override;		
		Result Load(Path path, TextureInternalPixelFormat internalFormat);
						
		Result Create(Vec2i size, TextureInternalPixelFormat internalFormat);
		Result Create(BitmapView bm);								
		Result Create(BitmapView bm, TextureInternalPixelFormat internalFormat);

		Result CopyPixels(Vec2i offset, BitmapView bm);		

		Bitmap GetBitmap() const;

		void GenerateMipmaps();

		Vec2i GetSize() const { return size; }		
		TextureInternalPixelFormat GetPixelFormat() const;

		inline unsigned GetHandle() const { return id; }

		Texture2D& operator=(const Texture2D&) = delete;
		Texture2D& operator=(Texture2D&&) noexcept;

		RESOURCE(Texture2D);
	};
}