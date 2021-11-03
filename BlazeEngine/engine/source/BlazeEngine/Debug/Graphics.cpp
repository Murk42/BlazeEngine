#include "BlazeEngine/Debug/Graphics.h"
#include "BlazeEngine/Graphics/GraphicsLibrary.h"
#include "BlazeEngine/Math/Math.h"
#include "BlazeEngine/Resources/Font/Font.h"
#include "source/BlazeEngine/Internal/Engine.h"
#include "BlazeEngine/DataStructures/Transform2D.h"
using namespace Blaze::Graphics::Core;

namespace Blaze
{
	namespace Renderer
	{
		BLAZE_API void SetActiveTextureSlot(uint slot);
		BLAZE_API void SelectTexture(Graphics::Core::Texture2D*);		
		BLAZE_API void SelectTexture(Graphics::Core::Texture2DArray*);		
		BLAZE_API void SelectTexture(Graphics::Core::TextureCubemap*);		
	}
	namespace Debug
	{				 
		void RenderTexture(Graphics::Core::Texture2D& texture, const Mat4f& mvp)
		{					
			if (engine->cache.renderTextureCache == nullptr)
				engine->cache.InitializeRenderTextureCache();
			Engine::Cache::RenderTextureCache& cache = *engine->cache.renderTextureCache;

			cache.program.SetUniform(cache.texUniform, 0);			
			cache.program.SetUniform(cache.matUniform, mvp);

			Renderer::SetActiveTextureSlot(0);
			Renderer::SelectTexture(&texture);

			Renderer::SelectVertexArray(&cache.va);
			Renderer::SelectProgram(&cache.program);

			Renderer::RenderTriangleArray(6, 0);			
		}
		void RenderTexture(Graphics::Core::Texture2D& texture, Vec2i pos, Vec2i size)
		{
			Vec2i viewportSize = Renderer::GetViewportSize();
			Mat4f mvp = Mat4f::Identity();
			
			mvp = mvp * Math::OrthographicMatrix<float>(0, viewportSize.x, 0, viewportSize.y, -1, 1);
			mvp = mvp * Math::TranslationMatrix<float>(Vec3f(pos.x, pos.y, 0));
			mvp = mvp * Math::ScalingMatrix<float>(Vec3f(size.x, size.y, 1));			

			RenderTexture(texture, mvp);
		}

		
		void Write(const String& text, ColorRGBA color, Mat4f& mat, Font& font)
		{		
			Engine::Cache::RenderTextCache& cache = *engine->cache.renderTextCache;

			Vec2f size;
			uint vertexCount;
			Font::CharacterVertex* vertices = font.GenerateVertices(text, vertexCount, size);
			cache.vb.AllocateDynamicStorage(BufferView(vertices, sizeof(Font::CharacterVertex) * vertexCount), GraphicsBufferDynamicStorageHint::StaticDraw);

			cache.program.SetUniform(cache.texUniform, 0);
			cache.program.SetUniform(cache.matUniform, mat);
			cache.program.SetUniform(cache.colorUniform, Vec4f(color.r, color.g, color.b, color.a));
			Renderer::SetActiveTextureSlot(0);
			Renderer::SelectTexture(&font.GetTexture());

			Renderer::SelectVertexArray(&cache.va);
			Renderer::SelectProgram(&cache.program);

			Renderer::SelectVertexBuffer(&cache.vb);


			Renderer::RenderPointArray(text.Size(), 0);
		}
		void Write(const String& text, ColorRGBA color, Vec2f pos, float scale, Align align, Font& font)
		{
			Engine::Cache::RenderTextCache& cache = *engine->cache.renderTextCache;

			Transform2D transform;
			transform.pos = pos;			
			transform.parentAlign = align;						
			transform.scale = scale;

			Vec2f size;
			uint vertexCount;
			Font::CharacterVertex* vertices = font.GenerateVertices(text, vertexCount, size);
			cache.vb.AllocateDynamicStorage(BufferView(vertices, sizeof(Font::CharacterVertex) * vertexCount), GraphicsBufferDynamicStorageHint::StaticDraw);
			transform.size = size * scale;

			transform.Update();			
			Vec2i viewportSize = Renderer::GetViewportSize();
			Mat4f proj = Math::OrthographicMatrix<float>(0, viewportSize.x, 0, viewportSize.y, -1, 1);

			cache.program.SetUniform(cache.texUniform, 0);
			cache.program.SetUniform(cache.matUniform, proj * transform.mat);
			cache.program.SetUniform(cache.colorUniform, Vec4f(color.r, color.g, color.b, color.a));
			Renderer::SetActiveTextureSlot(0);
			Renderer::SelectTexture(&font.GetTexture());			

			Renderer::SelectVertexArray(&cache.va);
			Renderer::SelectProgram(&cache.program);			

			Renderer::SelectVertexBuffer(&cache.vb);

			Renderer::RenderPointArray(text.Size(), 0);
		}
		
	}
}