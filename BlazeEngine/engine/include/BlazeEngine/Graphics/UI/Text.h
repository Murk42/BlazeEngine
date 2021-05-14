#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/StringView.h"
#include "BlazeEngine/DataStructures/Color.h"
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/DataStructures/Transform2D.h"
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
		Font(StringView path);
		~Font();

		bool Load(const StringView& path);

		friend class FontSize;
	};	

	class BLAZE_API FontSize
	{
	public:
		struct CharData {
			Vec2u textureSize;
			int textureOffset;
			Vec2i positionOffset;
			uint advance;
		};
	private:		
		Font* font;
		uint height;
		Texture2D texture;

		CharData characters[128];

	public:
		FontSize();
		FontSize(Font* font, uint height);

		void SetFontAndSize(Font* font, uint height);

		Font* GetFont() const { return font; }
		uint GetHeight() const { return height; }
		CharData GetCharData(char c) const { return characters[c]; }
		const Texture2D& GetTexture() const { return texture; }
	};

	class BLAZE_API Text
	{
	protected:
		FontSize* fontSize;		

		String string;
		Mesh mesh;
	public:
		Transform2D transform;

		Text();
		~Text();

		void SetFontSize(FontSize* fontSize);

		const Mesh& GetMesh() const { return mesh; }
		String GetString() const { return string; }				
		FontSize* GetFontSize() const { return fontSize; }

	};	

	class BLAZE_API NormalText : public Text
	{
		std::vector<Vertex<Vec2f, Vec2f, Vec2f, Vec2f>> vertices;
	public:
		NormalText();
		NormalText(FontSize* fontSize, StringView text);

		void SetString(StringView text);		

		void Render(BaseMaterial& material, const Color& color, const Mat4f& mvp);
	};

	class BLAZE_API ColoredText : public Text
	{					
		std::vector<Vertex<Vec2f, Vec2f, Vec2f, Vec2f, Vec4f>> vertices;		
	public:						
		ColoredText();
		ColoredText(FontSize* font, StringView text, const std::vector<Color>& colors);

		void SetString(StringView text, const std::vector<Color>& colors);

		void Render(BaseMaterial& material, const Mat4f& mvp);
	};
}