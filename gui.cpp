#include "gui.h"

GUI::~GUI() { }

GUI::GUI(Program *_gProgram, string font)
{
	gProgram = _gProgram;
	gQuad = new Quad(_gProgram);

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, font.c_str(), 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	FT_Set_Pixel_Sizes(face, 0, 48);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (GLubyte c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}

		///Flip
		unsigned int height = face->glyph->bitmap.rows;
		unsigned int width = face->glyph->bitmap.width;
		unsigned char *buffer = face->glyph->bitmap.buffer;

		unsigned int rows = height / 2;
		unsigned char* tempRow = (unsigned char*)malloc(width * sizeof(unsigned char));

		for (unsigned int rowIndex = 0; rowIndex < rows; rowIndex++)
		{
			memcpy(tempRow, buffer + rowIndex * width, width * sizeof(unsigned char));
			memcpy(buffer + rowIndex * width, buffer + (height - rowIndex - 1) * width, width * sizeof(unsigned char));
			memcpy(buffer + (height - rowIndex - 1) * width, tempRow, width * sizeof(unsigned char));
		}

		free(tempRow);
		///


		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			width,
			height,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			buffer
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

GLvoid GUI::Draw(mat4 orthoMatrix, GLuint texture, mat4 modelMatrix, GLfloat alpha)
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	gProgram->use();
	gProgram->setUniform("camera", orthoMatrix);
	gProgram->setUniform("alpha", alpha);
	gProgram->stopUsing();

	gQuad->Draw(texture, modelMatrix);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

GLvoid GUI::Draw(mat4 orthoMatrix, GLuint texture, GLfloat left, GLfloat top, GLfloat width, GLfloat height, GLfloat angle, GLfloat alpha)
{
	mat4 modelMatrix = 
		translate(mat4(), vec3(left, top, 0)) * 
		rotate(mat4(), radians(angle), vec3(0, 0, 1)) * 
		scale(mat4(), vec3(width, height, 1));

	Draw(orthoMatrix, texture, modelMatrix, alpha);
}

GLvoid GUI::DrawFont(mat4 orthoMatrix, string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color, GLfloat alpha)
{
	string::const_iterator c;
	GLfloat xOriginal = x;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat left = x + ch.Bearing.x * scale;
		GLfloat top = y + (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat width = ch.Size.x * scale;
		GLfloat height = ch.Size.y * scale;

		gProgram->use();
		gProgram->setUniform("color", color);
		gProgram->setUniform("isFont", true);
		gProgram->stopUsing();
		
		mat4 modelMatrix =
				translate(mat4(), vec3(left, top, 0)) *
				glm::scale(mat4(), vec3(width, -height, 1));

		Draw(orthoMatrix, ch.TextureID, modelMatrix, alpha);

		gProgram->use();
		gProgram->setUniform("color", vec3(1, 1, 1));
		gProgram->setUniform("isFont", false);
		gProgram->stopUsing();

		x += (ch.Advance >> 6) * scale;
	}
}