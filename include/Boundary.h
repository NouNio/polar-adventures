class Boundary {
private:
	std::vector<glm::vec3> points;
	std::vector <  float> xBounds, yBounds, zBounds;
	void resetPoints(){
		points.clear();
		for (size_t i = 0; i < xBounds.size(); i++)
		{
			for (size_t j = 0; j < yBounds.size(); j++)
			{
				for (size_t k = 0; k < zBounds.size(); k++)
				{
					points.push_back(glm::vec3(xBounds[i], yBounds[j], zBounds[k]));
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
				temp = transformation * temp;
				points[i] = glm::vec3(temp.x, temp.y, temp.z);
			}
		};

};