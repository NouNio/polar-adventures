/*
* Copyright 2020 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/
#include "Material.h"

/* --------------------------------------------- */
// Base material
/* --------------------------------------------- */

Material::Material(std::shared_ptr<GameShader> shader, glm::vec3 materialCoefficients, float alpha)
	: _shader(shader), _materialAmbient(glm::vec3(materialCoefficients.x)), _materialDiffuse(glm::vec3(materialCoefficients.y)), _materialSpecular(glm::vec3(materialCoefficients.z)),_alpha(alpha)
{
}

Material::Material(std::shared_ptr<GameShader> shader, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float alpha)
	: _shader(shader), _materialAmbient(glm::vec3(ambient)), _materialDiffuse(glm::vec3(diffuse)), _materialSpecular(glm::vec3(specular)), _alpha(alpha)
{
}

Material::~Material()
{
}

GameShader* Material::getShader()
{
	return _shader.get();
}


void Material::setUniforms(glm::vec3 originPoint)
{
	_shader->setUniform("materialAmbient", _materialAmbient);
	_shader->setUniform("materialDiffuse", _materialDiffuse);
	_shader->setUniform("materialSpecular", _materialSpecular);

	_shader->setUniform("specularAlpha", _alpha);
	_shader->setUniform("originpoint", originPoint);
}

/* --------------------------------------------- */
// Texture material
/* --------------------------------------------- */

TextureMaterial::TextureMaterial(std::shared_ptr<GameShader> shader, glm::vec3 materialCoefficients, float alpha, std::shared_ptr<TwoDTexture> diffuseTexture)
	: Material(shader, materialCoefficients, alpha), _diffuseTexture(diffuseTexture)
{
}
void TextureMaterial::setTexture(std::shared_ptr<TwoDTexture> texture) {
	_diffuseTexture->setTexture(texture);
}

TextureMaterial::~TextureMaterial()
{
}

void TextureMaterial::setUniforms(glm::vec3 originPoint)
{
	Material::setUniforms(originPoint);

	_diffuseTexture->bind(0);
	_shader->setUniform("diffuseTexture", 0);
}
