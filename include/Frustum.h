#include <glm/gtx/rotate_vector.hpp>
#include <vector>
//initially adapted from https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling, changed representations to fit project
//mainly inspired by https://bruop.github.io/improved_frustum_culling/, for this the approach of
class Frustum {
private:
	glm::vec3 viewDir;
	glm::vec3 up;
	glm::vec3 right;
	float hsize, vsize;
	glm::vec3 originPoint;
	glm::vec3 farPoint;
	glm::vec3 nearPoint;
	glm::mat4 projection;
	int renderedObjects = 0;
	float pitch, yaw, fov, hfov, near, far, aspect;
	std::vector<glm::vec3> normals;  //length 6 = top, bottom, right, left, front, back
	std::vector<float> viewSize;

	std::vector<glm::vec3> clipSpaceCoords ;
	std::vector<glm::vec3> clipNormals;
	//TODO: Fix this mess
	bool facesPlane(glm::vec3 planeOrigin, glm::vec3 normal, glm::vec4 point) {
		//FacesFromPlanes as in is inside frustum

		return isInbetween(point[0], -point[3], point[3]) && (point[1], -point[3], point[3]) && (point[2], 0.0, point[3]);
			
	};
	bool isInbetween(float value, float lower, float upper) {

		return(value <= upper && lower <= value);
		/*facesPlane(originPoint, normals[0], points) &&
			facesPlane(originPoint, normals[1], points) &&
			facesPlane(originPoint, normals[2], points) &&
			facesPlane(originPoint, normals[3], points) &&
			facesPlane(nearPoint, normals[4], points) &&
			facesPlane(farPoint, normals[5], points)*/
	};

	//´TODO ADD CHECK IF BOUNDARIES ARE NOT LARGER THAN frustum
	bool FacesFromAllPlanes(std::vector<glm::vec4> points) {
		bool isInAtLeastOnePlane = false;
		for (size_t i = 0; i < points.size(); i++)
		{


			isInAtLeastOnePlane = isInAtLeastOnePlane || (facesPlane(originPoint, normals[0], points[i])

				);
		}
		return isInAtLeastOnePlane;

	}

	void setPlanes() {
		//top, bottom, right, left, front, back
		/*normals.push_back(glm::cross(glm::rotate(viewDir, hfov * 0.5f, right), right));
		normals.push_back(-glm::cross(glm::rotate(viewDir, -hfov * 0.5f, right), right));
		normals.push_back(glm::cross(glm::rotate(viewDir, fov * 0.5f, up), up));
		normals.push_back(-glm::cross(glm::rotate(viewDir, -fov * 0.5f, up), up));
		normals.push_back(glm::normalize(viewDir));
		normals.push_back(glm::normalize(-viewDir));
		*/

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
		//std::ostringstream vts;
		//if (!normals.empty())
		//{
		//	// Convert all but the last element to avoid a trailing ","
		//	for (size_t i = 0; i < normals.size(); i++)
		//	{
		//		vts << normals[i].x << ' ' << normals[i].y << ' ' << normals[i].z << std::endl;
		//	}
	
		//}

		//std::cout << vts.str() << std::endl;
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
		viewDir(glm::normalize(viewDir)),
		projection(glm::perspective(fov, aspect, near, far))
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
		setFrustumVariables(fov, aspect);

	



	}
	void setFrustumVariables(float fov, float aspect) {
		float tang = glm::tan(0.5f * fov);

		viewSize = {
			//near - right
			tang * near * aspect,
			// near - top
			tang * near,
			// near - plane
			-near,
			// far - plane
			-far };
		/*
 (​−xnear​,xnear​,xnear​,−xnear​,
​					ynear​,ynear​,−ynear​,−ynear​
,					​znear ​znear ​znear ​znear​​))))​

*/
		float znear = viewSize[2];
		float zfar = viewSize[3];
		float xnear = viewSize[0];
		float ynear = viewSize[1];
		clipSpaceCoords = {
			glm::vec3(-1,1,1),
			glm::vec3(1,1,1),
			glm::vec3(1,-1,1),
			glm::vec3(-1,-1,1) };
		for (size_t i = 0; i < clipSpaceCoords.size(); i++)
		{
			clipSpaceCoords[i] = clipSpaceCoords[i] * glm::vec3(xnear, ynear, znear);
		}
		/*
		* n0​= (n1​= (n2​= (n3​= (n4​=
		( ​znear​, −znear​, 0,    0,     0,
		   ​0,     0,    −znear​, znear​,0,
		  ​−xnear, −xnear,​−ynear​,−ynear,​ 1​)))))​
		*/

		clipNormals = {
	glm::vec3(1,0,1),
	glm::vec3(-1,0,1),
	glm::vec3(0,-1,1),
	glm::vec3(0,1,1),
	//glm::vec3(0,0,1)
		 //,glm::vec3(0,0,-1)
		};
		for (size_t i = 0; i < clipNormals.size() - 1; i++)
		{
			float v = xnear;
			if (i >= (clipNormals.size() / 2)) v = ynear;
			clipNormals[i] = clipNormals[i] * glm::vec3(znear, znear, v);
		}



	}
	
	
	~Frustum() {

	}
	
	
	bool isInside(Boundary b, glm::mat4 model) {
		b.transform(model);
		bool isIn = true;

		/*
		ml​=ai,  ​nj, ​ai​xu, ai​xr, aixpk
		*/

		std::vector<glm::vec3> upandright;
		for (size_t i = 0; i < b.oobaxes.size(); i++)
		{
			upandright.push_back(b.oobaxes[i] * glm::vec3(0, -1, 1));
			upandright.push_back(b.oobaxes[i] * glm::vec3(1, 0, -1));
		}
		isIn = checkNear(b)&&
		//&&
			checkFrustumNormals(b)
			//check up and right, barely influence the result but do cause some false negatives, so nope, not doing that
			//&& checkAgainstCross(b, { glm::vec3(0,1,0), glm::vec3(-1,0,0)}, false)
			//&& checkAgainstCollectionOfAxes(b,upandright)
			//&& checkAgainstCross(b, clipNormals, false)
			;
		if(isIn){ increaseRenderedObjects(); }
		return isIn;
	};
	//*
	bool checkNear(Boundary b){
		glm::vec2 bounds = getProjectedBoundary(glm::vec3(0, 0, 1), b);
		glm::vec2 projBounds = glm::vec2( viewSize[3]  ,viewSize[2] );
		return (!checkForCondition(bounds,projBounds));
	}
	bool checkForCondition(glm::vec2 object, glm::vec2 projection) {
		return (object[0] > projection[1] || object[1] < projection[0]);
	}
	//*/
	bool checkFrustumNormals(Boundary b) {

		return checkAgainstCollectionOfAxes(b, clipNormals);
	}
	bool checkFrustum(Boundary b) {

		return checkAgainstCollectionOfAxes(b, clipNormals);
	}
	bool checkAxes(Boundary b) {
		for (size_t i = 0; i < b.oobaxes.size(); i++)
		{
			float center = glm::dot(b.oobaxes[i], b.center);
			float radius = b.ooblengths[i];
			glm::vec2 bounds = glm::vec2(center - radius, center + radius);
			glm::vec2 projectionBound = ProjectionBoundary(b.oobaxes[i]);
			if (checkForCondition(bounds, projectionBound)) return false;
		}
		return true;
	}
	bool checkAgainstCross(Boundary b,  std::vector<glm::vec3>  cross, bool isUPRIGHT) {
		std::vector<glm::vec3> values;
		for (size_t i = 0; i < cross.size(); i++)
		{
			for (size_t j = 0; j < b.oobaxes.size(); j++)
			{
				glm::vec3 value = glm::cross(cross[i], b.oobaxes[j]);
				const float epsilon = 1e-3;
				if (!isUPRIGHT){
					if ((glm::abs(value.x)) <= (glm::abs(value.y)) <= epsilon && (glm::abs(value.z)) <= epsilon) continue;
					}
					glm::vec2 bounds = getProjectedBoundary(value, b);
				glm::vec2 projectionBound = ProjectionBoundary(value);
				if (checkForCondition(bounds, projectionBound)) return false;
			}
		}

		return checkAgainstCollectionOfAxes(b, values);
	}
	bool checkAgainstCollectionOfAxes(Boundary b, std::vector<glm::vec3> axes) {
		for (size_t i = 0; i <axes.size(); i++) {


			glm::vec2 bounds = getProjectedBoundary(axes[i], b);
			glm::vec2 projectionBound = ProjectionBoundary(axes[i]);
			if (checkForCondition( bounds, projectionBound)) return false;

		}
		return true;
	}

		glm::vec2 ProjectionBoundary(glm::vec3 vector) {
			float znear = viewSize[2];
			float zfar = viewSize[3];
			float xnear = viewSize[0];
			float ynear = viewSize[1];


			float p = xnear * glm::abs(vector.x) + ynear * glm::abs(vector.y);
			glm::vec2 bound = glm::vec2();
			bound[0] = znear * vector.z - p;
			bound[1] = znear * vector.z + p;

			if (bound[0] < 0.0f) {
				bound[0] *= zfar / znear;
			}
			if (bound[1] > 0.0f) {
				bound[1] *= zfar / znear;
			}
			return bound;
		}
	glm::vec2 getProjectedBoundary(glm::vec3 axis, Boundary b ) {
		float projC = glm::dot(axis,b.center);
		// Projected size of OBB
		float radius = 0.0f;
		for (size_t i = 0; i < 3; i++) {
			// dot(M, axes[i]) == axes[i].z;
			radius += abs(glm::dot(axis, b.oobaxes[i]) * b.ooblengths[i]);
		}
		float oob_min = projC - radius;
		float oob_max = projC + radius;
		return glm::vec2(oob_min, oob_max);
	}


	void changeFOV(float fov) {
		this->fov = fov;



		setFrustumVariables(fov, aspect);


	}
	void update(float dpitch, float dyaw, glm::vec3 positionChange) 
	{
		
	};
	
	
	void reset(float pitch, float yaw, glm::vec3 position, glm::vec3 up, glm::vec3 viewDir) {
		resetRenderedObjects();

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