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
					points.push_back(comparison);
					if (glm::distance(points[0], comparison) > dist) {
						dist = glm::distance(points[0], comparison);
						maxdist = comparison;
				
					}
				}
			}
		}
	

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
		std::vector < float> xBound={0.0,0.0}, yBound={0.0,0.0}, zBound = { 0.0,0.0 };
		for (size_t i = 0; i < points.size(); i++)
		{
			if (points[i].x > xBound[1]) {
				xBounds[1] = points[i].x;
			}
			if (points[i].y > yBound[1]) {
				yBound[1] = points[i].y;
			}
			if (points[i].z > zBound[1]) {
				zBound[1] = points[i].z;
			}
			if (points[i].x < xBound[0]) {
				xBound[0] = points[i].x;
			}
			if (points[i].y < yBound[0]) {
				yBound[0] = points[i].y;
			}
			if (points[i].z < zBound[0]) {
				zBound[0] = points[i].z;
			}
		}

		points.clear();

		points.push_back(glm::vec3((xBound[0] + xBound[1]) / 2.0f, (yBound[0] + yBound[1]) / 2.0f, (zBound[0] + zBound[1]) / 2.0f));
		for (size_t i = 0; i < xBound.size(); i++)
		{
			for (size_t j = 0; j < yBound.size(); j++)
			{
				for (size_t k = 0; k < zBound.size(); k++)
				{
					glm::vec3 comparison(glm::vec3(xBound[i], yBound[j], zBound[k]));
					points.push_back(comparison);
				}
			}
		}
	};

};