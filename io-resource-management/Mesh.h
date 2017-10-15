/*
 * Mesh.h
 *
 *  Created on: 13 Dec 2015
 *      Author: james_000
 */

#ifndef IO_RESOURCE_MANAGEMENT_MESH_H_
#define IO_RESOURCE_MANAGEMENT_MESH_H_

#define GLEW_STATIC

#include <vector>
#include <map>

#include "../rendering/RenderingEngine.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include "../animation/AnimationEngine.h"

namespace osrm
{
	struct MeshSection	//Section of a mesh, has a material and list of indices relating to mesh data
	{
		osr::Material material;
		os_uint_t texID;
		GLuint numFaces;
		GLuint * faces;
		GLuint sizeBytesFaces;

		~MeshSection()
		{

		}
	};

	struct Mesh
	{
		GLfloat * positions, * uvs, * normals;
		GLuint numVertices;

		//Mesh section not required, each section has separate material and draw call
		std::vector<MeshSection> meshSections;

		//For normal mapped meshes
		GLfloat * tangents, * bitangents;

		//For skinned meshes
		osan::SkinnedMesh * skinnedMesh;
		//std::map<std::string, GLuint> boneMappings;
		//osan::SkeletalAnimation * animations;

		~Mesh()	//Free memory of all pointers
		{
			delete[] positions;
			delete[] uvs;
			delete[] normals;
			delete[] tangents;
			delete[] bitangents;
		}
	};
}

#endif /* IO_RESOURCE_MANAGEMENT_MESH_H_ */
