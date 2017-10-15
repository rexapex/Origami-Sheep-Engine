#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#include "../glm/glm.hpp"

#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace osi
{
	struct Character
	{
		glm::ivec2 size;
		glm::ivec2 bearing;
		glm::ivec2 advance;
	};

	struct Font
	{
		std::string name;
		std::map<GLchar, Character> chars;
		std::vector<unsigned char *> imgs;
		std::vector<int> widths, heights;
		GLuint texAtlasID;
		std::vector<glm::vec4> texCoords;
	};

	class FontManager
	{
	public:
		FontManager();
		virtual ~FontManager();

		void loadFont(std::string fontName);

		Font & getFont(size_t index) {return fonts[index];}
		std::vector<Font> & getFonts() {return fonts;}
	private:
		FT_Library ft;

		std::vector<Font> fonts;

		void initFontManager();
	};
}
