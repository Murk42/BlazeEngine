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

		friend class Text;
	};	

	class BLAZE_API Text
	{
	protected:
		Font* font;
		void* fontSizePtr;
		Vec2i size;

		String string;
		Mesh mesh;
	public:
		Text();
		~Text();

		void SetFont(Font* font, uint height);

		const Mesh& GetMesh() const { return mesh; }
		String GetString() const { return string; }
		Vec2i GetSize() const { return size; }
		const Texture2D* GetTexture() const;		

		void Render(BaseMaterial& material);
	};	

	class BLAZE_API NormalText : public Text
	{
		std::vector<Vertex<Vec2f, Vec2f, Vec2f, Vec2f>> vertices;
	public:
		void SetString(StringView text);		
	};

	class BLAZE_API ColoredText : public Text
	{					
		std::vector<Vertex<Vec2f, Vec2f, Vec2f, Vec2f, Vec4f>> vertices;		
	public:						
		void SetString(StringView text, const std::vector<Color>& colors);
	};
}