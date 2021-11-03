#include "source/BlazeEngine/Internal/Engine.h"
#include "BlazeEngine/Graphics/Renderer.h"

#include "BlazeEngine/Utilities/File.h"

namespace Blaze
{	
	Engine* engine = nullptr; 

	void Terminate();

	Engine::Engine()
	{
		Renderer.selectedVertexArray = &Renderer.defaultVertexArray;
	}
	Engine::~Engine()
	{
		Terminate();
	} 	

	void Engine::Cache::InitializeRenderSDFCache()
	{
		/*
			#version 330 core							
														
			in vec2 p_uv;								
														
			uniform sampler2D u_texture;				
														
			void main()									
			{											
				vec2 texSize = textureSize(u_texture, 0);
				float value = texture(u_texture, p_uv);
				float distance = 0;				
				for (float y = -5.0f; y <= 5.0f; ++y)
					for (float x = -5.0f; x <= 5.0f; ++x)
						if (x * x + y * y <= 5 * 5)
						{
							float value2 = texture(u_texture, p_uv + vec2(x, y) / texSize);
							if (value2 == 1.0f)
								distance = min(distance, 1.0f - (x * x + y * y) / 25);
						}

				vec4 color = vec4(sqrt(distance), 0, 0, 1);	
				gl_FragColor = color;											
			}																				
		*/
		/*
		const char* vertexSource = " \
			#version 330 core							\n\
														\n\
			layout(location = 0) in vec2 i_pos;			\n\
														\n\
			out vec2 p_uv;								\n\
														\n\
			void main()									\n\
			{											\n\
				p_uv = (i_pos + vec2(1, 1)) / 2;		\n\
				gl_Position = vec4(i_pos, 0, 1);		\n\
			}											\n\
			";
		
		File file = File("assets/SDFGenFragmentShader.frag", FileOpenMode::Read);
		Buffer buffer;
		buffer.Allocate(file.Size());
		file.Read(buffer);
		String fragmentSource = String((char*)buffer.Ptr(), buffer.Size());

		new (engine->cache.renderSDFCacheMemory) Engine::Cache::RenderSDFCache();
		engine->cache.renderSDFCache = (Engine::Cache::RenderSDFCache*)engine->cache.renderSDFCacheMemory;

		Engine::Cache::RenderSDFCache& cache = *engine->cache.renderSDFCache;
		Graphics::Core::VertexShader vertexShader;
		Graphics::Core::FragmentShader fragmentShader;
		vertexShader.ShaderSource(vertexSource);
		vertexShader.CompileShader();
		fragmentShader.ShaderSource(fragmentSource);
		fragmentShader.CompileShader();

		cache.program.LinkShaders({ &vertexShader, &fragmentShader });
		struct Vertex {
			Vec2f pos;
		} vertices[]{
			{ Vec2f(-1,-1) },
			{ Vec2f( 1,-1) },
			{ Vec2f(-1, 1) },
			{ Vec2f(-1, 1) },
			{ Vec2f( 1,-1) },
			{ Vec2f( 1, 1) }
		};

		cache.vb.AllocateStaticStorage(BufferView(vertices, sizeof(vertices)), Graphics::Core::GraphicsBufferStaticStorageHint::StaticStorage);

		cache.va.EnableVertexAttribute(0);
		cache.va.SetVertexAttributeData(0, Graphics::Core::VertexAttribute(&cache.vb, sizeof(Vertex), 0, Graphics::Core::VertexAttributeType::Float, 2, false, 0));

		cache.texUniform = cache.program.GetUniformLocation("u_texture");
		cache.limitUniform = cache.program.GetUniformLocation("limit");*/
	}
	void Engine::Cache::DeinitializeRenderSDFCache()
	{  

	}

	void Engine::Cache::InitializeRenderTextureCache()
	{
		/*
		const char* vertexSource = " \
			#version 330 core							\n\
														\n\
			layout(location = 0) in vec2 i_pos;			\n\
														\n\
			uniform mat4 u_MVP;							\n\
														\n\
			out vec2 p_uv;								\n\
														\n\
			void main()									\n\
			{											\n\
				p_uv = i_pos;							\n\
				gl_Position = u_MVP * vec4(i_pos, 0, 1);\n\
			}											\n\
		";
		const char* fragmentSource = " \
			#version 330 core							\n\
														\n\
			in vec2 p_uv;								\n\
														\n\
			uniform sampler2D u_texture;				\n\
														\n\
			void main()									\n\
			{											\n\
				vec4 color = texture(u_texture, vec2(p_uv.x, 1 - p_uv.y));	\n\
				gl_FragColor = color;					\n\
			}											\n\
		";

		new (engine->cache.renderTextureCacheMemory) Engine::Cache::RenderTextureCache();
		engine->cache.renderTextureCache = (Engine::Cache::RenderTextureCache*)engine->cache.renderTextureCacheMemory;

		Engine::Cache::RenderTextureCache& cache = *engine->cache.renderTextureCache;
		Graphics::Core::VertexShader vertexShader;
		Graphics::Core::FragmentShader fragmentShader;
		vertexShader.ShaderSource(vertexSource);
		vertexShader.CompileShader();
		fragmentShader.ShaderSource(fragmentSource);
		fragmentShader.CompileShader();
		cache.program.LinkShaders({ &vertexShader, &fragmentShader });

		struct Vertex {
			Vec2f pos;
		} vertices[]{
			{ Vec2f(0, 0) },
			{ Vec2f(0, 1) },
			{ Vec2f(1, 0) },
			{ Vec2f(1, 0) },
			{ Vec2f(0, 1) },
			{ Vec2f(1, 1) }
		};

		cache.vb.AllocateStaticStorage(BufferView(vertices, sizeof(vertices)));
		cache.va.EnableVertexAttribute(0);		
		cache.va.SetVertexAttributeData(0, Graphics::Core::VertexAttribute(&cache.vb, sizeof(Vertex), 0, Graphics::Core::VertexAttributeType::Float, 2, false, 0));

		cache.texUniform = cache.program.GetUniformLocation("u_texture");
		cache.matUniform = cache.program.GetUniformLocation("u_MVP");
		*/
	}
	void Engine::Cache::DeinitializeRenderTextureCache()
	{
		/*
		engine->cache.renderTextureCache->~RenderTextureCache();
		*/
	}

	void Engine::Cache::InitializeRenderTextCache()
	{
		/*
		const char* vertexSource = " \
				#version 330 core								 \n\
																 \n\
				layout(location = 0) in vec2 i_pos1;			 \n\
				layout(location = 1) in vec2 i_pos2;			 \n\
				layout(location = 2) in vec2 i_uv1;				 \n\
				layout(location = 3) in vec2 i_uv2;				 \n\
				layout(location = 4) in float i_layer;			 \n\
																 \n\
				out VS_OUT {									 \n\
					vec4 rect;									 \n\
					vec4 uv_rect;								 \n\
					float layer;								 \n\
				} vs_out;										 \n\
																 \n\
				void main()										 \n\
				{												 \n\
					vs_out.rect = vec4(i_pos1, i_pos2);			 \n\
					vs_out.uv_rect = vec4(i_uv1, i_uv2);		 \n\
					vs_out.layer = i_layer;						 \n\
				}												 \n\
		";
		const char* geometrySource = " \
			#version 330															\n\
																					\n\
			in VS_OUT {																\n\
				vec4 rect;															\n\
				vec4 uv_rect;														\n\
				float layer;														\n\
			} gs_in[];																\n\
																					\n\
			layout (points) in;														\n\
			layout (triangle_strip, max_vertices = 4) out;							\n\
																					\n\
			out vec2 p_uv;															\n\
																					\n\
			uniform mat4 u_MVP;														\n\
																					\n\
			void main()																\n\
			{																		\n\
				gl_Position = u_MVP * vec4(gs_in[0].rect.xy, gs_in[0].layer, 1);	\n\
				p_uv = gs_in[0].uv_rect.xy;											\n\
				EmitVertex();														\n\
																					\n\
				gl_Position = u_MVP * vec4(gs_in[0].rect.xw, gs_in[0].layer, 1);	\n\
				p_uv = gs_in[0].uv_rect.xw;											\n\
				EmitVertex();														\n\
																					\n\
				gl_Position = u_MVP * vec4(gs_in[0].rect.zy, gs_in[0].layer, 1);	\n\
				p_uv = gs_in[0].uv_rect.zy;											\n\
				EmitVertex();														\n\
																					\n\
				gl_Position = u_MVP * vec4(gs_in[0].rect.zw, gs_in[0].layer, 1);	\n\
				p_uv = gs_in[0].uv_rect.zw;											\n\
				EmitVertex();														\n\
																					\n\
				EndPrimitive();														\n\
			}																		\n\
		";
		const char* fragmentSource = " \
			#version 330 core												\n\
																			\n\
			in vec2 p_uv;													\n\
																			\n\
			uniform sampler2D u_texture;									\n\
			uniform vec4 u_color;											\n\
																			\n\
			void main()														\n\
			{																\n\
				float distance = texture(u_texture, p_uv).r;                \n\
																			\n\
				vec4 color = u_color;			                            \n\
				color.a = distance;											\n\
				if (color.a <= 0.0f)                                        \n\
					discard;                                                \n\
				gl_FragColor = color;							            \n\
			}																\n\
			";		

		new (engine->cache.renderTextCacheMemory) Engine::Cache::RenderTextCache();
		engine->cache.renderTextCache = (Engine::Cache::RenderTextCache*)engine->cache.renderTextCacheMemory;

		::Blaze::Engine::Cache::RenderTextCache& cache = *engine->cache.renderTextCache;
		Graphics::Core::VertexShader vertexShader;
		Graphics::Core::GeometryShader geometryShader;
		Graphics::Core::FragmentShader fragmentShader;		

		vertexShader.ShaderSource(vertexSource);
		vertexShader.CompileShader();
		geometryShader.ShaderSource(geometrySource);
		geometryShader.CompileShader();
		fragmentShader.ShaderSource(fragmentSource);
		fragmentShader.CompileShader();		

		cache.program.LinkShaders({ &vertexShader, &fragmentShader, &geometryShader });		

		cache.texUniform = cache.program.GetUniformLocation("u_texture");
		cache.matUniform = cache.program.GetUniformLocation("u_MVP");
		cache.colorUniform = cache.program.GetUniformLocation("u_color");		
	
		cache.va.EnableVertexAttribute(0);
		cache.va.EnableVertexAttribute(1);
		cache.va.EnableVertexAttribute(2);
		cache.va.EnableVertexAttribute(3);
		cache.va.EnableVertexAttribute(4);		
		cache.va.SetVertexAttributeData(0, Graphics::Core::VertexAttribute(&cache.vb, sizeof(float) * 9, 0, Graphics::Core::VertexAttributeType::Float, 2, false, 0));
		cache.va.SetVertexAttributeData(1, Graphics::Core::VertexAttribute(&cache.vb, sizeof(float) * 9, 0, Graphics::Core::VertexAttributeType::Float, 2, false, sizeof(float) * 2));
		cache.va.SetVertexAttributeData(2, Graphics::Core::VertexAttribute(&cache.vb, sizeof(float) * 9, 0, Graphics::Core::VertexAttributeType::Float, 2, false, sizeof(float) * 4));
		cache.va.SetVertexAttributeData(3, Graphics::Core::VertexAttribute(&cache.vb, sizeof(float) * 9, 0, Graphics::Core::VertexAttributeType::Float, 2, false, sizeof(float) * 6));
		cache.va.SetVertexAttributeData(4, Graphics::Core::VertexAttribute(&cache.vb, sizeof(float) * 9, 0, Graphics::Core::VertexAttributeType::Float, 1, false, sizeof(float) * 8));		
		*/
	}
	void Engine::Cache::DeinitializeRenderTextCache()
	{
		/*
		engine->cache.renderTextCache->~RenderTextCache();
		*/
	}
}