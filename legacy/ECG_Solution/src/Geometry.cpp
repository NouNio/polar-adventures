/*
* Copyright 2020 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/

#pragma once
#include "Geometry.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "vector"
#include <string>
#include <sstream>
#include <algorithm>


Geometry::Geometry(const Geometry& copied) :

	_modelMatrix(copied._modelMatrix),
	_elements(copied._elements),
	_material(copied._material)
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
};

void Geometry::generateBuffers() {
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

Geometry::~Geometry()
{
	glDeleteBuffers(1, &_vboPositions);
	glDeleteBuffers(1, &_vboNormals);
	glDeleteBuffers(1, &_vboUVs);
	glDeleteBuffers(1, &_vboIndices);
	glDeleteVertexArrays(1, &_vao);
}

Geometry::Geometry()
{
}

void Geometry::draw()
{
	GameShader* shader = _material->getShader();
	shader->use();

	shader->setUniform("modelMatrix", _modelMatrix);
	//shader->setUniform("normalMatrix", glm::mat3(glm::transpose(glm::inverse(_modelMatrix))));
	glm::vec4 transformedPosition = _modelMatrix*glm::vec4(glm::vec3(), 1);
	_material->setUniforms(glm::vec3(transformedPosition.x, transformedPosition.y, transformedPosition.z));

	glBindVertexArray(_vao);
	glDrawElements(GL_TRIANGLES, _elements, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Geometry::transform(glm::mat4 transformation)
{
	_modelMatrix = transformation * _modelMatrix;
}


void Geometry::resetModelMatrix()
{
	_modelMatrix = glm::mat4(1);
}
//assume that plane is planar, points are entered in cw order, p1 is bottom left
 GeometryData Geometry::createPane(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4){
	 GeometryData data;

	 data.positions.push_back(p1);
	 data.positions.push_back(p2);
	 data.positions.push_back(p3);
	 data.positions.push_back(p4);
//calculate normal;
	 glm::vec3 normal = glm::normalize(-glm::cross((p1 - p2), (p1 - p3)));
	 data.normals.push_back(normal);
	 data.normals.push_back(normal);
	 data.normals.push_back(normal);
	 data.normals.push_back(normal);
	

	 data.uvs.push_back(glm::vec2(0.0f, 0.0f));
	 data.uvs.push_back(glm::vec2(1.0f, 0.0f));
	 data.uvs.push_back(glm::vec2(1.0f, 1.0f));
	 data.uvs.push_back(glm::vec2(0.0f, 1.0f));

	 data.indices = {
		 0,3,2,
		 2,1,0};

	 return std::move(data);
 }

GeometryData Geometry::createCubeGeometry(float width, float height, float depth)
{
	GeometryData data;

	data.positions = {
		// front
		glm::vec3(-width / 2.0f, -height / 2.0f,  depth / 2.0f),
		glm::vec3(width / 2.0f, -height / 2.0f,  depth / 2.0f),
		glm::vec3(width / 2.0f, height / 2.0f,  depth / 2.0f),
		glm::vec3(-width / 2.0f, height / 2.0f,  depth / 2.0f),
		// back
		glm::vec3(width / 2.0f, -height / 2.0f,  -depth / 2.0f),
		glm::vec3(-width / 2.0f, -height / 2.0f,  -depth / 2.0f),
		glm::vec3(-width / 2.0f, height / 2.0f,  -depth / 2.0f),
		glm::vec3(width / 2.0f, height / 2.0f,  -depth / 2.0f),
		// right
		glm::vec3(width / 2.0f, -height / 2.0f,  depth / 2.0f),
		glm::vec3(width / 2.0f, -height / 2.0f,  -depth / 2.0f),
		glm::vec3(width / 2.0f, height / 2.0f,  -depth / 2.0f),
		glm::vec3(width / 2.0f, height / 2.0f,  depth / 2.0f),
		// left
		glm::vec3(-width / 2.0f, -height / 2.0f,  -depth / 2.0f),
		glm::vec3(-width / 2.0f, -height / 2.0f,  depth / 2.0f),
		glm::vec3(-width / 2.0f, height / 2.0f,  depth / 2.0f),
		glm::vec3(-width / 2.0f, height / 2.0f,  -depth / 2.0f),
		// top
		glm::vec3(-width / 2.0f, height / 2.0f,  -depth / 2.0f),
		glm::vec3(-width / 2.0f, height / 2.0f,  depth / 2.0f),
		glm::vec3(width / 2.0f, height / 2.0f,  depth / 2.0f),
		glm::vec3(width / 2.0f, height / 2.0f,  -depth / 2.0f),
		// bottom
		glm::vec3(-width / 2.0f, -height / 2.0f,  -depth / 2.0f),
		glm::vec3(width / 2.0f, -height / 2.0f,  -depth / 2.0f),
		glm::vec3(width / 2.0f, -height / 2.0f,  depth / 2.0f),
		glm::vec3(-width / 2.0f, -height / 2.0f,  depth / 2.0f)
	};

	data.normals = {
		// front
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, 1),
		// back
		glm::vec3(0, 0, -1),
		glm::vec3(0, 0, -1),
		glm::vec3(0, 0, -1),
		glm::vec3(0, 0, -1),
		// right
		glm::vec3(1, 0, 0),
		glm::vec3(1, 0, 0),
		glm::vec3(1, 0, 0),
		glm::vec3(1, 0, 0),
		// left
		glm::vec3(-1, 0, 0),
		glm::vec3(-1, 0, 0),
		glm::vec3(-1, 0, 0),
		glm::vec3(-1, 0, 0),
		// top
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		// bottom
		glm::vec3(0, -1, 0),
		glm::vec3(0, -1, 0),
		glm::vec3(0, -1, 0),
		glm::vec3(0, -1, 0)
	};

	data.uvs = {
		// front
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		glm::vec2(1, 1),
		glm::vec2(0, 1),
		// back
		glm::vec2(1, 1),
		glm::vec2(0, 1),
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		// right
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		glm::vec2(1, 1),
		glm::vec2(0, 1),
		// left
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		glm::vec2(1, 1),
		glm::vec2(0, 1),
		// top
		glm::vec2(0, 1),
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		glm::vec2(1, 1),
		// bottom
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		glm::vec2(1, 1),
		glm::vec2(0, 1)
	};

	data.indices = {
		// front
		0, 1, 2,
		2, 3, 0,
		// back
		4, 5, 6,
		6, 7, 4,
		// right
		8, 9, 10,
		10, 11, 8,
		// left
		12, 13, 14,
		14, 15, 12,
		// top
		16, 17, 18,
		18, 19, 16,
		// bottom
		20, 21, 22, 
		22, 23, 20
};

	return std::move(data);
}

GeometryData Geometry::createCylinderGeometry(unsigned int segments, float height, float radius)
{
	GeometryData data;


	// center vertices
	data.positions.push_back(glm::vec3(0, -height / 2.0f, 0));
	data.normals.push_back(glm::vec3(0, -1, 0));
	data.uvs.push_back(glm::vec2(0.5f, 0.5f));
	data.positions.push_back(glm::vec3(0, height / 2.0f, 0));
	data.normals.push_back(glm::vec3(0, 1, 0));
	data.uvs.push_back(glm::vec2(0.5f, 0.5f));

	// circle segments
	float angle_step = 2.0f * glm::pi<float>() / float(segments);
	for (unsigned int i = 0; i < segments; i++) {
		glm::vec3 circlePos = glm::vec3(
			glm::cos(i * angle_step) * radius,
			-height / 2.0f,
			glm::sin(i * angle_step) * radius
		);

		glm::vec2 squareToCirlceUV = glm::vec2(
			(circlePos.x / radius) * 0.5f + 0.5f,
			(circlePos.z / radius) * 0.5f + 0.5f
		);

		// bottom ring vertex
		data.positions.push_back(circlePos);
		data.positions.push_back(circlePos);
		data.normals.push_back(glm::vec3(0, -1, 0));
		data.normals.push_back(glm::normalize(circlePos - glm::vec3(0, -height / 2.0f, 0)));
		data.uvs.push_back(squareToCirlceUV);
		data.uvs.push_back(glm::vec2(i * angle_step / (2.0f * glm::pi<float>()), 0));

		// top ring vertex
		circlePos.y = height / 2.0f;
		data.positions.push_back(circlePos);
		data.positions.push_back(circlePos);
		data.normals.push_back(glm::vec3(0, 1, 0));
		data.normals.push_back(glm::normalize(circlePos - glm::vec3(0, height / 2.0f, 0)));
		data.uvs.push_back(squareToCirlceUV);
		data.uvs.push_back(glm::vec2(i * angle_step / (2.0f * glm::pi<float>()), 1));

		// bottom face
		data.indices.push_back(0);
		data.indices.push_back(2 + i * 4);
		data.indices.push_back(i == segments - 1 ? 2 : 2 + (i + 1) * 4);

		// top face
		data.indices.push_back(1);
		data.indices.push_back(i == segments - 1 ? 4 : (i + 2) * 4);
		data.indices.push_back((i + 1) * 4);

		// side faces
		data.indices.push_back(3 + i * 4);
		data.indices.push_back(i == segments - 1 ? 5 : 5 + (i + 1) * 4);
		data.indices.push_back(i == segments - 1 ? 3 : 3 + (i + 1) * 4);

		data.indices.push_back(i == segments - 1 ? 5 : 5 + (i + 1) * 4);
		data.indices.push_back(3 + i * 4);
		data.indices.push_back(5 + i * 4);
	}


	return std::move(data);
}

GeometryData Geometry::createSphereGeometry(unsigned int longitudeSegments, unsigned int latitudeSegments, float radius)
{
	GeometryData data;


	data.positions.push_back(glm::vec3(0.0f, radius, 0.0f));
	data.positions.push_back(glm::vec3(0.0f, -radius, 0.0f));
	data.normals.push_back(glm::vec3(0.0f, radius, 0.0f));
	data.normals.push_back(glm::vec3(0.0f, -radius, 0.0f));
	data.uvs.push_back(glm::vec2(0.0f, 0.0f));
	data.uvs.push_back(glm::vec2(0.0f, 1.0f));

	// first and last ring
	for (unsigned int j = 0; j < longitudeSegments; j++) {
		data.indices.push_back(0);
		data.indices.push_back(j == longitudeSegments - 1 ? 2 : (j + 3));
		data.indices.push_back(2 + j);

		data.indices.push_back(2 + (latitudeSegments - 2)*longitudeSegments + j);
		data.indices.push_back(j == longitudeSegments - 1 ? 2 + (latitudeSegments - 2)*longitudeSegments : 2 + (latitudeSegments - 2)*longitudeSegments + j + 1);
		data.indices.push_back(1);
	}

	// vertices and rings
	for (unsigned int i = 1; i < latitudeSegments; i++) {
		float verticalAngle = float(i) * glm::pi<float>() / float(latitudeSegments);
		for (unsigned int j = 0; j < longitudeSegments; j++) {
			float horizontalAngle = float(j) * 2.0f * glm::pi<float>() / float(longitudeSegments);
			glm::vec3 position = glm::vec3(
				radius * glm::sin(verticalAngle) * glm::cos(horizontalAngle),
				radius * glm::cos(verticalAngle),
				radius * glm::sin(verticalAngle) * glm::sin(horizontalAngle)
			);
			data.positions.push_back(position);
			data.normals.push_back(glm::normalize(position));
			data.uvs.push_back(glm::vec2(horizontalAngle / (2.0f * glm::pi<float>()), verticalAngle / glm::pi<float>()));

			if (i == 1) continue;

			data.indices.push_back(2 + (i - 1)*longitudeSegments + j);
			data.indices.push_back(j == longitudeSegments - 1 ? 2 + (i - 2)*longitudeSegments : 2 + (i - 2)*longitudeSegments + j + 1);
			data.indices.push_back(j == longitudeSegments - 1 ? 2 + (i - 1)*longitudeSegments : 2 + (i - 1)*longitudeSegments + j + 1);

			data.indices.push_back(j == longitudeSegments - 1 ? 2 + (i - 2)*longitudeSegments : 2 + (i - 2)*longitudeSegments + j + 1);
			data.indices.push_back(2 + (i - 1)*longitudeSegments + j);
			data.indices.push_back(2 + (i - 2)*longitudeSegments + j);
		}
	}

	return std::move(data);
}


//source is the filename of a .obj-file in the models without ".obj"
GeometryData Geometry::readModel(std::string source)
{
	//TODO: at this point we might as well have a parser: based on this? https://stackoverflow.com/questions/21120699/c-obj-file-parser. Assimp makes me lose my damn mind. Also https://en.wikipedia.org/wiki/Wavefront_.obj_file#Geometric_vertex,
//preconditions: .obj file no materials, thats handled externally, smooth shading disabled, triangulated faces



	GeometryData data;
	glm::mat4 modelMatrix = glm::mat4(1);
	std::vector<glm::vec3> vertexPositions;
	std::vector<glm::vec2> vertexUVs;
	std::vector<glm::vec3> vertexNormals;
	unsigned int currentIndex=0;


	std::ifstream model("assets\\models\\"+source+".obj");
	std::string src((std::istreambuf_iterator<char>(model)), std::istreambuf_iterator<char>());

	std::istringstream origStream(src);


	std::string curLine;
	while (std::getline(origStream, curLine))
	{
		// Only add non empty lines.
		if (!curLine.empty()) {
			std::vector<string> seperated;
			std::string word;
			for (auto x : curLine)
			{
				if (x == ' ')
				{
					seperated.push_back(word);
					word = "";
				}
				else {
					word = word + x;
				}
			}
			seperated.push_back(word);
			//std::stod()
			std::string reference=seperated[0];
			if (reference.at(0) == 'v') {
				if (reference.length() > 1) {
			
					if (reference.at(1) == 'n') {
							glm::vec3 normal;
							normal.x = std::stod(seperated[1]);
							normal.y = std::stod(seperated[2]);
							normal.z = std::stod(seperated[3]);

							vertexNormals.push_back(normal);

						}
					if (reference.at(1) == 't') {
						glm::vec2 uv;
						uv.x = std::stod(seperated[1]);
						uv.y = std::stod(seperated[2]);

						vertexUVs.push_back(uv);

					}
					
				}
				else {
					glm::vec3 position;
					position.x = std::stod(seperated[1]);
					position.y = std::stod(seperated[2]);
					position.z = std::stod(seperated[3]);

					vertexPositions.push_back(position);
				}
			}
			if (reference.at(0) == 'f') {
			//each face has 3 vertices
				for (size_t i = 1; i < 4; i++)
				{
					std::vector<std::string> vertexindexes;
					word = "";
					//seperate indexes
					for (auto x : seperated[i])
					{
						if (x == '/')
						{
							vertexindexes.push_back(word);
							word = "";
						}
						else {
							word = word + x;
						}

					}
					vertexindexes.push_back(word);
					data.positions.push_back(vertexPositions[(std::stoi(vertexindexes[0])-1)]);
					data.uvs.push_back(vertexUVs[(std::stoi(vertexindexes[1]) - 1)]);
					data.normals.push_back(vertexNormals[(std::stoi(vertexindexes[2]) - 1)]);
					data.indices.push_back(currentIndex++);
					vertexindexes.clear();
				}
			
				}
			
			seperated.clear();
		}
			
	}


	return data;
}
	//  */

