#pragma once
#include "Geometry.h"
//file is a proxy
//*
static std::shared_ptr<GameShader> textureShader() {return std::make_shared<GameShader>("texture.vert", "texture.frag");}; 


static std::shared_ptr<TwoDTexture> woodTexture() {return std::make_shared<TwoDTexture>("wood_texture.dds");
};

static std::shared_ptr<TwoDTexture> tileTexture() { return std::make_shared<TwoDTexture>("tiles_diffuse.dds"); };


static std::shared_ptr<TextureMaterial> tileTextureMaterial(std::shared_ptr<GameShader> shader) {
	return std::make_shared<TextureMaterial>(shader, glm::vec3(0.1f, 0.7f, 0.3f), 8.0f, tileTexture());
};
// Create materials
static std::shared_ptr<TextureMaterial> woodTextureMaterial(std::shared_ptr<GameShader> shader) {
	return std::make_shared<TextureMaterial>(shader, glm::vec3(0.1f, 0.1f, 0.1f), 2.0f, woodTexture());
};

static Geometry cuboid_cube(std::shared_ptr<GameShader> shader) {
	return Geometry(glm::mat4(1), Geometry::readModel("cuboid v2 (current)"), woodTextureMaterial(shader));
};
static Geometry cuboid_rail(std::shared_ptr<GameShader> shader) {return  Geometry(glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(0, 0, 1)), Geometry::readModel("rail fragment v2 (better fitting shape)"), woodTextureMaterial(shader));
};  //Geometry::readModel("");
static Geometry cuboid_twist(std::shared_ptr<GameShader> shader){ return  Geometry(glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(0, 0, 1)), Geometry::readModel("rail twist element.obj"), woodTextureMaterial(shader));
};  
static Geometry cuboid_holder(std::shared_ptr<GameShader> shader) {return Geometry(glm::mat4(1), Geometry::readModel("edgepiece (rework doesn't fit with the twist)"), woodTextureMaterial(shader));
};
//*/


