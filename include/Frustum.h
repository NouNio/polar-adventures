#include <glm/gtx/rotate_vector.hpp>
#include <vector>
//adapted from https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling, changed representations to fit project
class Frustum {
private:
	glm::vec3 viewDir;
	glm::vec3 up;
	glm::vec3 right;
	float hsize, vsize;
	glm::vec3 originPoint;
	glm::vec3 farPoint;
	glm::vec3 nearPoint;
	int renderedObjects = 0;
	float pitch, yaw, fov, hfov, near, far, aspect;
	std::vector<glm::vec3> normals;  //length 6 = top, bottom, right, left, front, back


	//TODO: Fix this mess
	bool facesPlane(glm::vec3 planeOrigin, glm::vec3 normal, std::vector<glm::vec3> points) {
		//FacesFromPlanes as in is inside frustum
		return 
			//(glm::dot(glm::normalize(normal-planeOrigin), (points[0] - planeOrigin)) >= -glm::distance(points[0], points[1]));
			(glm::dot(glm::normalize(normal), (points[0] - planeOrigin)) >= -glm::distance(points[0], points[1]));
	};

	//´TODO ADD CHECK IF BOUNDARIES ARE NOT LARGER THAN frustum
	bool FacesFromAllPlanes(std::vector<glm::vec3> points) {
		return (
			facesPlane(originPoint, normals[0], points)&&
			facesPlane(originPoint, normals[1], points)&&
			facesPlane(originPoint, normals[2], points)&&
			facesPlane(originPoint, normals[3], points)&&
			facesPlane(nearPoint, normals[4], points)&&
			facesPlane(farPoint, normals[5], points)
			);

	}

	void setPlanes() {
		//top, bottom, right, left, front, back
		normals.push_back(glm::cross(glm::rotate(viewDir, hfov * 0.5f, right), right));
		normals.push_back(-glm::cross(glm::rotate(viewDir, -hfov * 0.5f, right), right));
		normals.push_back(glm::cross(glm::rotate(viewDir, fov * 0.5f, up), up));
		normals.push_back(-glm::cross(glm::rotate(viewDir, -fov * 0.5f, up), up));
		normals.push_back(glm::normalize(viewDir));
		normals.push_back(glm::normalize(-viewDir));

		/*
		normals.push_back(glm::normalize(glm::cross(right, (farPoint - originPoint) - up * (vsize * 0.5f))));
		normals.push_back(glm::normalize(glm::cross((farPoint - originPoint) + up * (vsize * 0.5f), right)));
		normals.push_back(glm::normalize(glm::cross(up, (farPoint - originPoint) + right * (hsize * 0.5f))));
		normals.push_back(glm::normalize(glm::cross((farPoint - originPoint) - right * (hsize * 0.5f), up)));
		normals.push_back(glm::normalize(viewDir));
		normals.push_back(glm::normalize(-viewDir));
		
		
		normals.push_back(glm::cross(glm::rotate(viewDir,hfov*0.5f,right),right));
		normals.push_back(-glm::cross(glm::rotate(viewDir, -hfov*0.5f, right), right));
		normals.push_back(glm::cross(glm::rotate(viewDir, fov*0.5f, up), up));
		normals.push_back(-glm::cross(glm::rotate(viewDir, -fov*0.5f, up), up));
		normals.push_back(glm::normalize(viewDir));
		normals.push_back(glm::normalize( - viewDir));
		*/
		std::ostringstream vts;
		if (!normals.empty())
		{
			// Convert all but the last element to avoid a trailing ","
			for (size_t i = 0; i < normals.size(); i++)
			{
				vts << normals[i].x << ' ' << normals[i].y << ' ' << normals[i].z << std::endl;
			}
	
		}

		std::cout << vts.str() << std::endl;
	}


public:
	Frustum(float fov, float near, float far, float aspect, glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 viewDir = glm::vec3(0, 0, -1), glm::vec3 up = glm::vec3(0, 1, 0), float pitch = 0, float yaw = 0) :
		fov(fov),
		near(near),
		far(far),
		pitch(pitch),
		yaw(yaw),
		aspect(aspect),
		right(glm::normalize(glm::cross(up, viewDir))),
		up(glm::normalize(up)),
		viewDir(glm::normalize(viewDir))
	{
		hfov = 2 * atan(tan(fov / 2) * aspect);
		//ensure everything is 
		viewDir = glm::normalize(viewDir);
		up = glm::normalize(up);
		right = glm::normalize(glm::cross(viewDir, up));
		hsize = 2 * far * tanf(fov * 0.5f);
		vsize = hsize * aspect;
		originPoint = position;
		nearPoint = originPoint + near * viewDir;
		farPoint = originPoint + far * viewDir;

		setPlanes();
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
		if (FacesFromAllPlanes(points)) { increaseRenderedObjects(); }
		return FacesFromAllPlanes(points);
	};
	
	void changeFOV(float fov) {
		this->fov = fov;
		hsize = 2 * far * tanf(fov * 0.5f);
		vsize = hsize * aspect;
		setPlanes();
	}
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
		normals.clear();
		setPlanes();

	};


	int getRenderedObjects() {
		return renderedObjects;
	}


	void resetRenderedObjects() {
		renderedObjects = 0;
	}


	void increaseRenderedObjects() {
		renderedObjects++;
	}
};