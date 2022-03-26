/*
* Copyright 2020 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/
#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "GameShader.h"
#include "TwoDTexture.h"



/*!
 * Base material
 */
class Material
{
protected:
	/*!
	 * The shader used for rendering this material
	 */
	std::shared_ptr<GameShader> _shader;
	/*!
	 * The material's coefficients (x = ambient, y = diffuse, z = specular)
	 */
	glm::vec3 _materialAmbient;
	glm::vec3 _materialDiffuse;
	glm::vec3 _materialSpecular;
	/*!
	 * Alpha value, i.e. the shininess constant
	 */
	float _alpha;

public:
	/*!
	 * Base material constructor
	 * @param shader: The shader used for rendering this material
	 * @param materialCoefficients: The material's coefficients (x = ambient, y = diffuse, z = specular)
	 * @param alpha: Alpha value, i.e. the shininess constant
	 */	Material(std::shared_ptr<GameShader> shader, glm::vec3 materialCoefficients, float alpha);
	 Material(std::shared_ptr<GameShader> shader, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float alpha);

	 virtual ~Material();
	 void setShader(std::shared_ptr<GameShader> shader) {
		 _shader = shader;
	 };
	/*!
	 * @return The shader associated with this material
	 */
	GameShader* getShader();

	/*!
	 * Sets this material's parameters as uniforms in the shader
	 */
	virtual void setUniforms(glm::vec3 originPoint);
};


/*!
 * Texture material
 */
class TextureMaterial : public Material
{
protected:
	/*!
	 * The diffuse texture of this material
	 */
	std::shared_ptr<TwoDTexture> _diffuseTexture;

public:

	void TextureMaterial::setTexture(std::shared_ptr<TwoDTexture> texture);
	/*!
	 * Texture material constructor
	 * @param shader: The shader used for rendering this material
	 * @param materialCoefficients: The material's coefficients (x = ambient, y = diffuse, z = specular)
	 * @param alpha: Alpha value, i.e. the shininess constant
	 * @param diffuseTexture: The diffuse texture of this material
	 */
	TextureMaterial(std::shared_ptr<GameShader> shader, glm::vec3 materialCoefficients, float alpha, std::shared_ptr<TwoDTexture> diffuseTexture);
	
	virtual ~TextureMaterial();

	/*!
	 * Set's this material's parameters as uniforms in the shader
	 */
	virtual void setUniforms(glm::vec3 originPoint);
};
