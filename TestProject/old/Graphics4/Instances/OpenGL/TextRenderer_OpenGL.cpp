#include "pch.h"
#include "TextRenderer_OpenGL.h"
#include "TextRenderTask_OpenGL.h"
#include "OpenGLWrapper/OpenGLContext.h"

namespace Graphics4::OpenGL
{
	struct FontResolutionAtlasData
	{
		struct GlyphData
		{
			Vec2f uv1, uv2;
		};

		Map<UnicodeChar, GlyphData> glyphsData;
		OpenGLWrapper::Texture2D atlas;
	};

	static FontResolutionAtlasData* GetAtlasData(FontResolution* fontResolution, Font* font)
	{
		auto it = fontResolution->dataMap.Find("TextRenderer_OpenGL");

		if (!it.IsNull())
			return it.GetValue<FontResolutionAtlasData>();

		it = fontResolution->dataMap.Insert("TextRenderer_OpenGL", FontResolutionAtlasData()).iterator;

		FontResolutionAtlasData* ptr = it.GetValue<FontResolutionAtlasData>();
		FontGlyphRenderers::AntialiasedFontGlyphRenderer glyphRenderer;
		FontGlyphBitmaps bitmaps{ *font, *fontResolution, glyphRenderer };
		Vec2u atlasSize;

		bitmaps.CreateAtlas([&](Vec2u size) {
			atlasSize = size;
			ptr->atlas.Create(size, OpenGLWrapper::TextureInternalPixelFormat::R8);
		}, [&](UnicodeChar ch, Vec2u offset, BitmapView bm) {

			FontResolutionAtlasData::GlyphData glyphData;
			glyphData.uv1 = Vec2f(offset) / Vec2f(atlasSize);
			glyphData.uv2 = Vec2f(offset + bm.GetSize()) / Vec2f(atlasSize);
			std::swap(glyphData.uv1.y, glyphData.uv2.y);

			ptr->glyphsData.Insert(ch, glyphData);
			ptr->atlas.CopyPixels(offset, bm);
		});

		ptr->atlas.SetSettings({
			.min = OpenGLWrapper::TextureSampling::Linear,
			.mag = OpenGLWrapper::TextureSampling::Linear
			});

		return ptr;
	}

	static constexpr auto VectexBufferVertexCount = 256;
	static constexpr auto VertexBufferSize = VectexBufferVertexCount * 32;

	TextRenderer_OpenGL::TextRenderer_OpenGL(Graphics4::OpenGL::GraphicsContext_OpenGL& graphicsContext)
		: graphicsContext(graphicsContext)
	{
		OpenGLWrapper::VertexShader vert{ "assets/shaders/text.vert" };
		OpenGLWrapper::FragmentShader frag{ "assets/shaders/text.frag" };

		program.LinkShaders({ &vert, &frag });		
		program.SetUniform(1, 0);

		vb.Allocate(nullptr, VertexBufferSize, OpenGLWrapper::ImmutableGraphicsBufferMapAccess::Write, OpenGLWrapper::ImmutableGraphicsBufferMapType::None);		

		va.EnableVertexAttribute(0);
		va.SetVertexAttributeFormat(0, OpenGLWrapper::VertexAttributeType::Float, 2, false, 0);
		va.SetVertexAttributeBuffer(0, &vb, 32, 0);
		va.EnableVertexAttribute(1);
		va.SetVertexAttributeFormat(1, OpenGLWrapper::VertexAttributeType::Float, 2, false, 8);
		va.SetVertexAttributeBuffer(1, &vb, 32, 0);
		va.EnableVertexAttribute(2);
		va.SetVertexAttributeFormat(2, OpenGLWrapper::VertexAttributeType::Float, 4, false, 16);
		va.SetVertexAttributeBuffer(2, &vb, 32, 0);		
	}	
	bool TextRenderer_OpenGL::CreateTask(const TextRenderData& renderData, Dynamic::TextRenderTaskBase& task)
	{		
		if (auto taskPtr = task.TryCast<TextRenderTask_OpenGL>())
			return CreateTask(renderData, *taskPtr);			

		if (!task.IsImplemented())		
			return CreateTask(renderData, task.SetImplementation<TextRenderTask_OpenGL>());		
		
		return false;
	}
	bool TextRenderer_OpenGL::CreateTask(const TextRenderData& renderData, TextRenderTask_OpenGL& task)
	{
		FontResolutionAtlasData* atlasData = GetAtlasData(renderData.fontResolution, renderData.font);				
		FontGlyphBitmaps glyphBitmaps;

		renderData.textLayouter->SetText(renderData.text, *renderData.font, *renderData.fontResolution);
		auto charactersData = renderData.textLayouter->GetCharactersData();
		auto& glyphsData = renderData.fontResolution->GetGlyphsData();
		auto& atlasGlyphsData = atlasData->glyphsData;
		
		uintMem vertexCount = 0;
		Array<Vertex> vertices{ charactersData.Count() * 6 };

		float scale = (float)renderData.height / renderData.fontResolution->GetFontHeight();
		for (uintMem i = 0; i < charactersData.Count(); ++i)
		{
			TextLayouterBase::CharacterData characterData = charactersData[i];
			FontResolutionGlyphData glyphData;
			FontResolutionAtlasData::GlyphData atlasGlyphData;

			{
				auto it = glyphsData.Find(characterData.character);
				if (it.IsNull())
					continue;
				glyphData = it->value;
			}

			{
				auto it = atlasGlyphsData.Find(characterData.character);
				if (it.IsNull())
					continue;
				atlasGlyphData = it->value;
			}

			ColorRGBAf color = (i >= renderData.colors.Count() ? 0xffffffff : renderData.colors[i]); 						
			Vec2f pos1 = characterData.pos * scale , pos2 = (characterData.pos + characterData.size) * scale;
			Vec2f uv1 = atlasGlyphData.uv1, uv2 = atlasGlyphData.uv2;

			vertices[vertexCount + 0] = { Vec2f(pos1.x, pos1.y), Vec2f(uv1.x, uv1.y), color };			
			vertices[vertexCount + 1] = { Vec2f(pos2.x, pos1.y), Vec2f(uv2.x, uv1.y), color };
			vertices[vertexCount + 2] = { Vec2f(pos1.x, pos2.y), Vec2f(uv1.x, uv2.y), color };
			vertices[vertexCount + 3] = { Vec2f(pos1.x, pos2.y), Vec2f(uv1.x, uv2.y), color };
			vertices[vertexCount + 4] = { Vec2f(pos2.x, pos1.y), Vec2f(uv2.x, uv1.y), color };
			vertices[vertexCount + 5] = { Vec2f(pos2.x, pos2.y), Vec2f(uv2.x, uv2.y), color };
			vertexCount += 6;
		}

		task.renderCache.vertices = std::move(vertices);
		task.renderCache.atlas = &atlasData->atlas;
		task.renderCache.transform = renderData.transform;
		task.renderCache.size = renderData.textLayouter->GetSize() * scale;
		task.renderer = this;

		return true;
	}	
	Mat4f CalculateModelMatrix(const TextTransformData& transform, Vec2f size, Vec2u renderArea)
	{
		return
			Mat4f::TranslationMatrix(Vec3f(transform.rotationPivot * size + transform.anchorPivot * Vec2f(renderArea) + transform.pos - transform.pivot * size, 0)) *
			Mat4f::RotationMatrixAxisZ(transform.rotation) *
			Mat4f::TranslationMatrix(Vec3f(-transform.rotationPivot * size, 0));
	}
	void TextRenderer_OpenGL::Render(const TextRenderCache& renderCache, Graphics4::OpenGL::Framebuffer_OpenGL& renderTarget)
	{
		renderTarget.MakeActive();

		OpenGLWrapper::SelectProgram(&program);
		OpenGLWrapper::SelectVertexArray(&va);		
		OpenGLWrapper::SetActiveTextureSlot(0);
		OpenGLWrapper::SelectTexture(renderCache.atlas);

		Vec2u renderArea = Vec2u(renderTarget.GetSize());
		OpenGLWrapper::SetRenderArea(Vec2i(), (Vec2i)renderArea);		
		Mat4f proj = Mat4f::OrthographicMatrix(0, renderArea.x, 0, renderArea.y, -1, 1);

		program.SetUniform(0, proj * CalculateModelMatrix(renderCache.transform, renderCache.size, renderArea));

		OpenGLWrapper::Fence fence{ };
		fence.SetFence();
		uintMem offset = 0;
		while (offset != renderCache.vertices.Count())
		{
			uintMem count = renderCache.vertices.Count() % VectexBufferVertexCount;			

			void* vbMap = vb.MapBufferRange(0, VertexBufferSize, OpenGLWrapper::ImmutableGraphicsBufferMapOptions::InvalidateBuffer | OpenGLWrapper::ImmutableGraphicsBufferMapOptions::ExplicitFlush);
			memcpy(vbMap, renderCache.vertices.Ptr() + offset, sizeof(Vertex) * count);			
			vb.UnmapBuffer();
			//vb.FlushBufferRange(0, count * sizeof(Vertex));

			OpenGLWrapper::RenderPrimitiveArray(OpenGLWrapper::PrimitiveType::Triangles, 0, count);
			fence.SetFence();			

			if (fence.BlockClient(1.0) == OpenGLWrapper::FenceReturnState::TimeoutExpired)
			{
				Debug::Logger::LogWarning("Blaze Graphics API", "Fence timeout");
				return;
			}

			offset += count;
		}	
	}
}
