#pragma once

#define GLEW_STATIC
#define GLM_FORCE_RADIANS

#include <windows.h>

//origami sheep
#include "Globals.h"

//opengl, glew & glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

//resource loading libs
#include <SOIL.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//glm headers
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/detail/type_mat.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/detail/type_mat3x3.hpp"
#include "glm/detail/type_mat4x4.hpp"
#include "glm/detail/type_vec3.hpp"