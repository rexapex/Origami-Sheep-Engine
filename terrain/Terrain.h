#pragma once

#include "../glm/glm.hpp"
#include <vector>
#include <iostream>

namespace osutil
{
	struct GridCell
	{
		glm::vec3 p[8];
		double val[8];
	};

	struct Triangle
	{
		glm::vec3 p[3];
	};

	void marchingCubes(GridCell * grid, int gridSize, std::vector<Triangle> & tris);
	int marchPolygonise(GridCell & cell, double isoLvl, Triangle * tris);
	glm::vec3 marchVertexInterp(double isoLvl, glm::vec3 p1, glm::vec3 p2, double valp1, double valp2);
}
