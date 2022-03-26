/*
* Copyright 2020 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/
#pragma once


#include <vector>
#include <memory>
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "GL\glew.h"
#include "Material.h"
#include "GameShader.h"



//*/



struct GeometryData {
	/*!
	 * Vertex positions
	 */
	std::vector<glm::vec3> positions;
	/*!
	 * Geometry indices
	 */
	std::vector<unsigned int> indices;
	/*!
	 * Vertex normals
	 */
	std::vector<glm::vec3> normals;
	/*!
	 * Vertex UV coordinates
	 */
	std::vector<glm::vec2> uvs;
};

class Geometry
{
private:
	void generateBuffers();
protected:
	/*!
	Used only for copyConstructor
	*/
	GeometryData data;
	/*!
	 * Vertex array object
	 */
	GLuint _vao;
	/*!
	 * Vertex buffer object that stores the vertex positions
	 */
	GLuint _vboPositions;
	/*!
	 * Vertex buffer object that stores the vertex normals
	 */
	GLuint _vboNormals;
	/*!
	 * Vertex buffer object that stores the vertex UV coordinates
	 */
	GLuint _vboUVs;
	/*!
	 * Vertex buffer object that stores the indices
	 */
	GLuint _vboIndices;

	/*!
	 * Number of elements to be rendered
	 */
	unsigned int _elements;

	/*!
	 * Material of the geometry object
	 */


	 /*!
	  * Model matrix of the object
	  */


public:

	std::shared_ptr<TextureMaterial> _material;
	//look i dont care anymore
	glm::mat4 _modelMatrix;
	/*!
	 * Geometry object constructor
	 * Creates VAO and VBOs and binds them
	 * @param modelMatrix: model matrix of the object
	 * @param data: data for the geometry object
	 * @param material: material of the geometry object
	 */
	Geometry(const Geometry& copied);
	Geometry(glm::mat4 modelMatrix, GeometryData& data, std::shared_ptr<TextureMaterial> material)
		: _elements(data.indices.size()), _modelMatrix(modelMatrix), _material(material), data(data)
	{
		// create VAO
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		// create positions VBO
		glGenBuffers(1, &_vboPositions);
		glBindBuffer(GL_ARRAY_BUFFER, _vboPositions);
		glBufferData(GL_ARRAY_BUFFER, data.positions.size() * sizeof(glm::vec3), data.positions.data(), GL_STATIC_DRAW);

		// bind positions to location 0
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		// create normals VBO
		glGenBuffers(1, &_vboNormals);
		glBindBuffer(GL_ARRAY_BUFFER, _vboNormals);
		glBufferData(GL_ARRAY_BUFFER, data.normals.size() * sizeof(glm::vec3), data.normals.data(), GL_STATIC_DRAW);

		// bind normals to location 1
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		// create uvs VBO
		glGenBuffers(1, &_vboUVs);
		glBindBuffer(GL_ARRAY_BUFFER, _vboUVs);
		glBufferData(GL_ARRAY_BUFFER, data.uvs.size() * sizeof(glm::vec2), data.uvs.data(), GL_STATIC_DRAW);

		// bind uvs to location 2
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

		// create and bind indices VBO
		glGenBuffers(1, &_vboIndices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vboIndices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(unsigned int), data.indices.data(), GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	}
	~Geometry();
	Geometry();
	/*!
	 * Draws the object
	 * Uses the shader, sets the uniform and issues a draw call
	 */
	void draw();

	/*!
	 * Transforms the object, i.e. updates the model matrix
	 * @param transformation: the transformation matrix to be applied to the object
	 */
	void transform(glm::mat4 transformation);

	/*!
	 * Resets the model matrix to the identity matrix
	 */
	void resetModelMatrix();
	void Geometry::setTexture(std::shared_ptr<TwoDTexture> texture) {
		_material->setTexture(texture);
	}
	void Geometry::setShader(std::shared_ptr<GameShader> shader) {
		_material->setShader(shader);
	}
	void Geometry::invertNormals() {
		for (size_t i = 0; i < data.normals.size(); i++)
		{
			data.normals[i] *= -1;
			glDeleteBuffers(1, &_vboNormals);
			glGenBuffers(1, &_vboNormals);
			glBindBuffer(GL_ARRAY_BUFFER, _vboNormals);
			glBufferData(GL_ARRAY_BUFFER, data.normals.size() * sizeof(glm::vec3), data.normals.data(), GL_STATIC_DRAW);
		}
	}


	/*!
	 * Creates a cube geometry
	 * @param width: width of the cube
	 * @param height: height of the cube
	 * @param depth: depth of the cube
	 * @return all cube data
	 */
	static GeometryData createCubeGeometry(float width, float height, float depth);
	/*!
	 * Creates a cylinder geometry
	 * @param segments: number of segments of the cylinder
	 * @param height: height of the cylinder
	 * @param radius: radius of the cylinder
	 * @return all cylinder data
	 */
	static GeometryData createCylinderGeometry(unsigned int segments, float height, float radius);
	/*!
	 * Creates a sphere geometry
	 * @param longitudeSegments: number of longitude segments of the sphere
	 * @param latitudeSegments: number of latitude segments of the sphere
	 * @param radius: radius of the sphere
	 * @return all sphere data
	 */
	static GeometryData createSphereGeometry(unsigned int longitudeSegments, unsigned int latitudeSegments, float radius);

	//assume that plane is planar, points are entered in cw order, p1 is bottom left
	static GeometryData createPane(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4);
	static GeometryData readModel(std::string source);
};