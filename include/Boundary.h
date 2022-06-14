//iiinitially partly inspired by https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling, changed representations to fit project
//obb-calculation adapted from https://bruop.github.io/improved_frustum_culling/, for this the approach of

class Boundary {
private:
	std::vector<glm::vec3> points;
	std::vector <  float> xBounds, yBounds, zBounds;

	void resetPoints() {
		points.clear();
		glm::vec3 maxdist;
		float dist = 0.0f;
		points.push_back(glm::vec3((xBounds[0] + xBounds[1]) / 2.0f, (yBounds[0] + yBounds[1]) / 2.0f, (zBounds[0] + zBounds[1]) / 2.0f));
		for (size_t i = 0; i < xBounds.size(); i++)
		{
			for (size_t j = 0; j < yBounds.size(); j++)
			{
				for (size_t k = 0; k < zBounds.size(); k++)
				{
					glm::vec3 comparison(glm::vec3(xBounds[i], yBounds[j], zBounds[k]));
					points.push_back(comparison);
					if (glm::distance(points[0], comparison) > dist) {
						dist = glm::distance(points[0], comparison);
						maxdist = comparison;

					}
				}
			}
		}

		minCorner = glm::vec3(xBounds[0], yBounds[0], zBounds[0]);
		maxCorner = glm::vec3(xBounds[1], yBounds[1], zBounds[1]);
		center = glm::vec3(xBounds[0] + xBounds[1], xBounds[0] + yBounds[1], zBounds[0] + zBounds[1]) / 2.0f;
		oobaxes.clear();
		ooblengths.clear();
		oobaxes.push_back(glm::vec3(maxCorner.x - minCorner.x, 0, 0));
		oobaxes.push_back(glm::vec3(0, maxCorner.y - minCorner.y, 0));
		oobaxes.push_back(glm::vec3(0, 0, maxCorner.z - minCorner.z));
		ooblengths.push_back(glm::length(oobaxes[0]) / 2.0);
		ooblengths.push_back(glm::length(oobaxes[1]) / 2.0);
		ooblengths.push_back(glm::length(oobaxes[2]) / 2.0);
		oobaxes[0] = glm::normalize(oobaxes[0]);
		oobaxes[1] = glm::normalize(oobaxes[1]);
		oobaxes[2] = glm::normalize(oobaxes[2]);
	}


public:
	//OOB
	glm::vec3 minCorner;
	glm::vec3 maxCorner;
	glm::vec3 center;
	//onb
	std::vector<glm::vec3> oobaxes;
	std::vector<float> ooblengths;
	Boundary(std::vector < float> xBounds, std::vector < float > yBounds, std::vector < float > zBounds) :xBounds(xBounds), yBounds(yBounds), zBounds(zBounds) {

		resetPoints();
	}


	void reset() {
		resetPoints();
	}


	std::vector<glm::vec3> getPoints() {
		return points;
	}


	void transform(glm::mat4 transformation) {
		glm::vec4 temp;
		for (size_t i = 0; i < points.size(); i++)
		{
			temp = glm::vec4(points[i].x, points[i].y, points[i].z, 1.0f);
			temp = transformation * temp;
			points[i] = glm::vec3(temp.x, temp.y, temp.z);
		}
		temp = glm::vec4(minCorner.x, minCorner.y, minCorner.z, 1.0f);
		temp = transformation * temp;
		minCorner = glm::vec3(temp.x, temp.y, temp.z);;
		temp = glm::vec4(maxCorner.x, maxCorner.y, maxCorner.z, 1.0f);
		temp = transformation * temp;
		maxCorner = glm::vec3(temp.x, temp.y, temp.z);;

		temp = glm::vec4(center.x, center.y, center.z, 1.0f);
		temp = transformation * temp;
		center = glm::vec3(temp.x, temp.y, temp.z);;
		oobaxes[0] = points[4] - points[0];
		oobaxes[1] = points[2] - points[0];
		oobaxes[2] = points[1] - points[0];
		ooblengths.clear();
		ooblengths.push_back(glm::length(oobaxes[0]) / 2.0);
		ooblengths.push_back(glm::length(oobaxes[1]) / 2.0);
		ooblengths.push_back(glm::length(oobaxes[2]) / 2.0);

		oobaxes[0] = glm::normalize(oobaxes[0]);
		oobaxes[1] = glm::normalize(oobaxes[1]);
		oobaxes[2] = glm::normalize(oobaxes[2]);
		/*
		temp = glm::vec4(minCorner.x, minCorner.y, minCorner.z, 1.0f);
		temp = transformation * temp;
		minCorner = glm::vec3(temp.x, temp.y, temp.z);;
		temp = glm::vec4(maxCorner.x, maxCorner.y, maxCorner.z, 1.0f);
		temp = transformation * temp;
		maxCorner = glm::vec3(temp.x, temp.y, temp.z);;

		temp = glm::vec4(center.x, center.y, center.z, 1.0f);
		temp = transformation * temp;
		center = glm::vec3(temp.x, temp.y, temp.z);;
		for (size_t i = 0; i < oobaxes.size(); i++)
		{
			temp = glm::vec4(oobaxes[i].x, oobaxes[i].y, oobaxes[i].z, 1.0f);
			temp = transformation * temp;
			oobaxes[i] = glm::vec3(temp.x, temp.y, temp.z);
		}
		ooblengths.clear();
		ooblengths.push_back(glm::length(oobaxes[0])/1.0);
		ooblengths.push_back(glm::length(oobaxes[1])/1.0);
		ooblengths.push_back(glm::length(oobaxes[2])/1.0);

		oobaxes[0] = glm::normalize(oobaxes[0]);
		oobaxes[1] = glm::normalize(oobaxes[1]);
		oobaxes[2] = glm::normalize(oobaxes[2]);
	};
	*/

	};
};