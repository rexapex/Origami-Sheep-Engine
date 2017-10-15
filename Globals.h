#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <string>

//typedef GLvoid      void;
typedef GLchar      os_char_t;
typedef GLbyte	    os_byte_t;
typedef GLubyte	    os_ubyte_t;
typedef GLboolean   os_bool_t;
typedef GLshort     os_short_t;
typedef GLushort    os_ushort_t;
typedef GLuint      os_uint_t;
typedef GLint       os_int_t;
typedef GLint64     os_long_t;
typedef GLuint64    os_ulong_t;
typedef GLfloat     os_float_t;
typedef GLdouble    os_double_t;
typedef GLsizeiptr  os_sizeiptr_t;
typedef GLintptr	os_intptr_t;

enum class OSError
{
	NONE = 0, SETUP = 1, RENDER = 2, PHYSICS = 4	//TODO - Make this better later on
};

const std::string os_emptystring = "";
