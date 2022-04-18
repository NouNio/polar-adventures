#include <glm/gtx/rotate_vector.hpp>
class Frustum {
private:
	glm::vec3 viewDir;
	glm::vec3 up;
	glm::vec3 right;

	glm::vec3 originPoint;
	glm::vec3 farPoint;
	glm::vec3 nearPoint;
	int renderedObjects = 0;
	float pitch, yaw, fov, hfov, near, far;
	std::vector<glm::vec3> normals;  //length 6 = top, bottom, right, left, front, back


	//TODO: Fix this mess
	bool facesPlane(glm::vec3 planeOrigin, glm::vec3 normal, glm::vec3 point) {
		//FacesFromPlanes as in is inside frustum
		return (glm::dot(point, normal) - glm::distance(planeOrigin, glm::vec3(0.0f)))>0.0f;
	};

	
	bool FacesFromAllPlanes(std::vector<glm::vec3> points) {
		for (size_t i = 0; i < points.size(); i++)
		{
			if (!facesPlane(originPoint, normals[0],points[i]))continue;
			if (!facesPlane(originPoint, normals[1], points[i]))continue;
			if (!facesPlane(originPoint, normals[2], points[i]))continue;
			if (!facesPlane(originPoint, normals[3], points[i]))continue;
			if (!facesPlane(nearPoint, normals[4], points[i]))continue;
			if (!facesPlane(farPoint, normals[5], points[i]))continue;
			renderedObjects++;
			return false;
		}
		return true;
	}




public:
	Frustum(float fov, float near, float far, float aspect, glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 viewDir = glm::vec3(0, 0, -1), glm::vec3 up = glm::vec3(0, 1, 0), float pitch = 0, float yaw = 0) :
		fov(fov),
		near(near),
		far(far),
		pitch(pitch),
		yaw(yaw),
		right(glm::normalize(glm::cross(up, viewDir))),
		up(glm::normalize(up)),
		viewDir(glm::normalize(viewDir))
	{
		//ensure everything is 
		viewDir = glm::normalize(viewDir);
		up = glm::normalize(up);
		right = glm::normalize(glm::cross(viewDir, up));
		 hfov = fov / aspect;
		originPoint = position;
		nearPoint = originPoint + near * viewDir;
		farPoint = originPoint + far * viewDir;
		normals.push_back(glm::rotate(-up, -hfov / 2, right));
		normals.push_back(glm::rotate(up, hfov / 2, right));
		normals.push_back(glm::rotate(-right, -fov / 2, up));
		normals.push_back(glm::rotate(right, fov / 2, up));
		normals.push_back(viewDir);
		normals.push_back(-viewDir);
		this->fov = fov;
		this->pitch = pitch;
		this->yaw = yaw;
		this->viewDir = viewDir;
		this->up = up;
		this->near=near;
		this->far = far;
	}
	
	
	~Frustum() {

	}
	
	
	bool isInside(std::vector<glm::vec3> points) {
		return !FacesFromAllPlanes(points);
	};
	
	
	void update(float dpitch, float dyaw, glm::vec3 positionChange) 
	{
		for (size_t i = 0; i < normals.size(); i++){
			normals[i] = glm::rotate(normals[i], dpitch, up);
		}

		right = glm::rotate(right, dpitch, up);
		viewDir = glm::rotate(viewDir, dpitch, up);
		
		for (size_t i = 0; i < normals.size(); i++){
			normals[i] = glm::rotate(normals[i], dyaw, right);
		}
		
		up = glm::rotate(right, dpitch, right);
		yaw += yaw;
		pitch += pitch;
	};
	
	
	void reset(float pitch, float yaw, glm::vec3 position, glm::vec3 up, glm::vec3 viewDir) {
		resetRenderedObjects();
		this->yaw = yaw;
		this->pitch = pitch;
		this->viewDir = glm::normalize(viewDir);
		this->up = glm::normalize(up);
		right = glm::normalize(glm::cross(viewDir, up));
		originPoint = position;
		nearPoint = originPoint + near * viewDir;
		farPoint = originPoint + far * viewDir;
		normals[0]=(glm::rotate(-up, -hfov / 2, right));
		normals[1]=(glm::rotate(up, hfov / 2, right));
		normals[2]=(glm::rotate(-right, -fov / 2, up));
		normals[3]=(glm::rotate(right, fov / 2, up));
		normals[4]=(viewDir);
		normals[5]=(-viewDir);

	};


	int getRenderedObjects() {
		return renderedObjects;
	}


	void resetRenderedObjects() {
		renderedObjects = 0;
	}
};