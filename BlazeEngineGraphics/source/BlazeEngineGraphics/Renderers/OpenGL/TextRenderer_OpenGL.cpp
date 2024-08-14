#include "pch.h"
#include "BlazeEngineGraphics/Renderers/OpenGL/TextRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLShader.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLFence.h"
#include "BlazeEngineGraphics/Files/shaders.h"

namespace Blaze::Graphics::OpenGL
{
	static constexpr auto InstanceBufferInstanceCount = 256;
	static constexpr auto InstanceBufferSize = InstanceBufferInstanceCount * 56;

	struct AtlasData
	{
		struct GlyphData
		{
			Vec2f uv1;
			Vec2f uv2;
		};

		OpenGLWrapper::Texture2D atlas;
		Map<UnicodeChar, GlyphData> glyphData;
	};

	AtlasData* CreateAtlas(Font* font, uint pixelFontHeight, StringView dataName)
	{
		AtlasData* atlasData = font->dataMap.Insert<AtlasData>("OpenGLAtlasData").iterator.GetValue<AtlasData>();

		Vec2f atlasSize;

		FontMetrics* fontMetrics = &font->GetMetrics(pixelFontHeight);
		FontGlyphRenderers::AntialiasedFontGlyphRenderer glyphRenderer;		
		fontMetrics->CreateAtlas(CharacterSet::ASCIICharacterSet(), glyphRenderer,
			[&](Vec2u size) {
				atlasSize = (Vec2f)size;

				atlasData->atlas.Create((Vec2u)size, OpenGLWrapper::TextureInternalPixelFormat::R8, {
					.min = OpenGLWrapper::TextureSampling::Linear,
					.mag = OpenGLWrapper::TextureSampling::Linear,
					.mip = OpenGLWrapper::TextureSampling::Nearest,
					.textureLevelCount = 1
					});				
			}, [&](UnicodeChar ch, Vec2u offset, BitmapView bm) {
				AtlasData::GlyphData gd{
					.uv1 = (Vec2f)offset / atlasSize,
					.uv2 = (Vec2f)(offset + bm.GetSize()) / atlasSize
				};
				std::swap(gd.uv1.y, gd.uv2.y);
				atlasData->glyphData.Insert(ch, gd);
				atlasData->atlas.SetPixels(offset, bm, 0);
				}
			);

		return atlasData;
	}

	AtlasData* GetAtlasData(Font* font, uint pixelFontHeight)
	{
		String name = "OpenGLAtlasData" + StringParsing::Convert(pixelFontHeight);
		auto atlasIt = font->dataMap.Find(name);

		if (atlasIt.IsNull())
			return CreateAtlas(font, pixelFontHeight, name);
		else if (AtlasData* atlasData = atlasIt.GetValue<AtlasData>())
			return atlasData;
		else
		{
			font->dataMap.Erase(name);
			return CreateAtlas(font, pixelFontHeight, name);
		}
	}

	void TextRenderCache_OpenGL::CreateNew(TextRenderer_OpenGL& renderer, const Array<TextRenderData_OpenGL>& renderData)
	{
		groups.Clear();		

		for (auto& rd : renderData)
		{
			AtlasData* atlasData = GetAtlasData(rd.font, (uint)std::round(rd.atlasFontHeight));

			Vec2f right = Vec2f(Math::Cos(rd.rotation), Math::Sin(rd.rotation));
			Vec2f up = { -right.y, right.x };
			

			auto& instances = groups.Insert(&atlasData->atlas).iterator->value;			
			
			instances.ReserveAdditional(rd.characterData.Count());
			for (uintMem i = 0; i < rd.characterData.Count(); ++i)
			{
				auto& charData = rd.characterData[i];				

				auto glyphDataIt = atlasData->glyphData.Find(charData.character);
				if (glyphDataIt.IsNull())
					continue;
				auto glyphData = glyphDataIt->value;

				Vec2f uv1 = glyphData.uv1;
				Vec2f uv2 = glyphData.uv2;				

				Vec2f offset = charData.pos;
				Vec2f size = charData.size;
				ColorRGBAf color = charData.color;
				UnicodeChar character = charData.character;

				Vec2f p1 = right * offset.x + up * offset.y + rd.pos;
				Vec2f p2 = p1 + right * size.x;
				Vec2f p3 = p1 + up * size.y;

				Instance instance{
					.color = color, 
					.p1 = p1, .p2 = p2, .p3 = p3,
					.uv1 = uv1, .uv2 = uv2,
				};

				instances.AddBack(instance);
			}
		}
	}

	TextRenderer_OpenGL::TextRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext)
		: graphicsContext(graphicsContext)
	{		
		Blaze::Graphics::OpenGLWrapper::VertexShader vert;
		vert.ShaderSource(StringView((const char*)text_vert_file, text_vert_size));
		vert.CompileShader();

		Blaze::Graphics::OpenGLWrapper::FragmentShader frag;
		frag.ShaderSource(StringView((const char*)text_frag_file, text_frag_size));
		frag.CompileShader();

		program.LinkShaders({ &vert, &frag });

		uint8 vertices[6]{
			0, 1, 2, 2, 1, 3
		};

		vertexBuffer.Allocate(vertices, sizeof(vertices));
		instanceBuffer.Allocate(nullptr, InstanceBufferSize, Graphics::OpenGLWrapper::ImmutableGraphicsBufferMapAccess::Write, Graphics::OpenGLWrapper::ImmutableGraphicsBufferMapType::None);

		va.EnableVertexAttribute(0);
		va.SetVertexAttributeFormat(0, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Uint8, 1, false, 0);
		va.SetVertexAttributeBuffer(0, &vertexBuffer, sizeof(TextRenderCache_OpenGL::Vertex), 0);

		va.EnableVertexAttribute(1);
		va.SetVertexAttributeFormat(1, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 4, false, 0);
		va.SetVertexAttributeBuffer(1, &instanceBuffer, sizeof(TextRenderCache_OpenGL::Instance), 0);
		va.SetVertexAttributeDivisor(1, 1);
		va.EnableVertexAttribute(2);
		va.SetVertexAttributeFormat(2, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 2, false, 16);
		va.SetVertexAttributeBuffer(2, &instanceBuffer, sizeof(TextRenderCache_OpenGL::Instance), 0);
		va.SetVertexAttributeDivisor(2, 1);
		va.EnableVertexAttribute(3);
		va.SetVertexAttributeFormat(3, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 2, false, 24);
		va.SetVertexAttributeBuffer(3, &instanceBuffer, sizeof(TextRenderCache_OpenGL::Instance), 0);
		va.SetVertexAttributeDivisor(3, 1);
		va.EnableVertexAttribute(4);
		va.SetVertexAttributeFormat(4, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 2, false, 32);
		va.SetVertexAttributeBuffer(4, &instanceBuffer, sizeof(TextRenderCache_OpenGL::Instance), 0);
		va.SetVertexAttributeDivisor(4, 1);
		va.EnableVertexAttribute(5);
		va.SetVertexAttributeFormat(5, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 2, false, 40);
		va.SetVertexAttributeBuffer(5, &instanceBuffer, sizeof(TextRenderCache_OpenGL::Instance), 0);
		va.SetVertexAttributeDivisor(5, 1);
		va.EnableVertexAttribute(6);
		va.SetVertexAttributeFormat(6, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 2, false, 48);
		va.SetVertexAttributeBuffer(6, &instanceBuffer, sizeof(TextRenderCache_OpenGL::Instance), 0);
		va.SetVertexAttributeDivisor(6, 1);
	}

	void TextRenderer_OpenGL::Render(const TextRenderCache_OpenGL& renderCache, Vec2u targetSize)
	{
		graphicsContext.SelectProgram(&program);
		graphicsContext.SelectVertexArray(&va);
		graphicsContext.SetActiveTextureSlot(0);

		Vec2u renderArea = Vec2u(targetSize);
		Mat4f proj = Mat4f::OrthographicMatrix(0, (float)targetSize.x, 0, (float)targetSize.y, -1, 1);	

		program.SetUniform(0, proj);
		program.SetUniform(1, 0);

		Blaze::Graphics::OpenGLWrapper::Fence fence{ };
		fence.SetFence();

		for (auto& group : renderCache.groups)
		{
			graphicsContext.SelectTexture(group.key);

			uintMem offset = 0;
			while (offset != group.value.Count())
			{
				uintMem count = group.value.Count() % InstanceBufferInstanceCount;

				if (fence.BlockClient(1.0) == Blaze::Graphics::OpenGLWrapper::FenceReturnState::TimeoutExpired)
				{
					Debug::Logger::LogWarning("Blaze Graphics API", "Fence timeout");
					return;
				}

				void* instanceBufferMap = instanceBuffer.MapBufferRange(0, InstanceBufferSize, Blaze::Graphics::OpenGLWrapper::ImmutableGraphicsBufferMapOptions::InvalidateBuffer | Blaze::Graphics::OpenGLWrapper::ImmutableGraphicsBufferMapOptions::ExplicitFlush);

				memcpy(instanceBufferMap, group.value.Ptr() + offset, sizeof(Instance) * count);
				instanceBuffer.FlushBufferRange(0, sizeof(Instance) * count);
				instanceBuffer.UnmapBuffer();

				graphicsContext.RenderInstancedPrimitiveArray(Blaze::Graphics::OpenGLWrapper::PrimitiveType::Triangles, 0, 6, 0, group.value.Count());

				fence.SetFence();

				offset += count;
			}
		}
	}	
}