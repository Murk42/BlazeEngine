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
		void* ptr;
		std::vector<void*> sizes;

		void* AddSize(uint height);
		void RemoveSize(void* size);
	public:
		Font();
		~Font();

		bool Load(const StringView& path);

		friend class TextRenderer;
	};	

	class BLAZE_API TextRenderer
	{			
		Font* font;
		void* fontSizePtr;		
		Vec2i size;

		String string;
		std::vector<Vertex<Vec2f, Vec2f, Vec2f, Vec2f, Vec4f>> vertices;
		Mesh mesh;
	public:		
		TextRenderer();
		~TextRenderer();
				
		void SetFont(Font* font, uint characterHeight);
		void SetString(StringView text);
		void SetColors(const std::vector<Color>& colors);
		
		const Mesh& GetMesh() const { return mesh; }
		String GetString() const { return string; }
		Vec2i GetSize() const { return size; }
		const Texture2D* GetTexture() const;
	};
}