#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/StringView.h"
#include "BlazeEngine/DataStructures/Color.h"
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/Graphics/Texture.h"
#include "BlazeEngine/Graphics/Material.h"
#include "BlazeEngine/Graphics/Mesh.h"
#include <vector>

namespace Blaze
{	
	class BLAZE_API Font
	{
		struct Size {
			Size() = default;
			Size(const Size&);
			struct Character
			{
				Vec2i size;
				Vec2i offset;
				uint advance = 0;
				uint uv_offset = 0;
			} characters[128];

			Texture2D texture;
			uint height = 0;
			uint useCount = 0;

			Size& operator=(const Size&) noexcept;
		};
		void* ptr;
		std::vector<Size*> sizes;


		Size* AddSize(uint height);
		void RemoveSize(Size* size);
	public:
		Font();
		~Font();

		bool Load(const StringView& path);

		friend class TextRenderer;
	};

	class BLAZE_API TextRenderer
	{		
		Font* font;				
		Font::Size* size;	
		int width;
		
		std::vector<Vertex<Vec2f, Vec2f, Vec2f, Vec2f, float>> vertices;
		Mesh mesh;
	public:		
		TextRenderer();
		~TextRenderer();
				
		void SetFont(Font* font, uint characterHeight);
		void SetString(StringView text);
		
		Mesh& GetMesh() { return mesh; }
		Vec2i GetSize() { return Vec2i(width, size->height); }
		Texture2D* GetTexture() { return &size->texture; }
	};
}