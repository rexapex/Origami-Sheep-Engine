#include "FontManager.h"

namespace osi
{
	FontManager::FontManager()
	{
		initFontManager();
	}

	FontManager::~FontManager()
	{
		FT_Done_FreeType(ft);
	}



	void FontManager::initFontManager()
	{
		if(FT_Init_FreeType(&ft))
			std::cerr << "Error: Could not initialise freetype library" << std::endl;
	}



	void FontManager::loadFont(std::string fontName)
	{
		FT_Face face;	//Face = Font
		if(FT_New_Face(ft, fontName.c_str(), 0, &face))
		{
			std::cerr << "Error: Freetype failed to load font: " << fontName << std::endl;
			return;
		}

		FT_Set_Pixel_Sizes(face, 0, 48);	//Width = 0, therefore width calculated based on height

		Font font;
		font.name = fontName;

		for(GLubyte c = 0; c < 128; c++)
		{
			if(FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cerr << "Error: Freetype failed to load glyph: " << c << std::endl;
				continue;
			}

			int width = static_cast<int>(face->glyph->bitmap.width);
			int height = static_cast<int>(face->glyph->bitmap.rows);

			//unsigned char * buffer = (unsigned char *)malloc(width * height * sizeof(unsigned char));

			unsigned char * buffer = new unsigned char[width * height * sizeof(unsigned char)];

			if(face->glyph->bitmap.buffer != nullptr)
				memcpy(buffer, face->glyph->bitmap.buffer, width * height);

			font.imgs.push_back(buffer);
			font.widths.push_back(width);
			font.heights.push_back(height);

			//Store character in map
			Character character = {glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top), glm::vec2(face->glyph->advance.x, face->glyph->advance.y)};

			font.chars.insert(std::pair<GLchar, Character>(c, character));
		}

		fonts.push_back(font);

		FT_Done_Face(face);
	}
}
