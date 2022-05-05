//adapted from https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling, changed representations to fit project
class Boundary {
private:
	std::vector<glm::vec3> points;
	std::vector <  float> xBounds, yBounds, zBounds;
	
	void resetPoints(){
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
					if (glm::distance(points[0], comparison) > dist) {
						dist = glm::distance(points[0], comparison);
						maxdist = comparison;
					}
				}
			}
		}
		points.push_back(maxdist);

	}


public:
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
		for (size_t i = 0; i < points.size(); i++)
		{
			glm::vec4 temp = glm::vec4(points[i].x, points[i].y, points[i].z, 1.0f);
			temp = transformation*temp ;
			points[i] = glm::vec3(temp.x, temp.y, temp.z);
		}
	};

};