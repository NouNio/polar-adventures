#pragma once
#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <chrono>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

#include <Constants.h>
#include <Shader.h>


#define MAX_PARTICLES 6000


// the paricle system has 3 buffers:
// 1 for the mesh vertices, 6 vec3, i.e. 2 triangles, i.e. 1 rectangle
// 2 for the particle centers (VBO)
// 3 for the particles colors (VBO)


struct Particle {
	glm::vec3 pos, speed;
	unsigned char r, g, b, a; 
	float size, angle, weight;
	float life;     // if < 0 particle is unused so far or dead
	float camDist;  // squared distance to camera, -1.0 of dead

	void reset() {
		life = -1;
		camDist = -1;
	}

	bool outOfXBounds() {
		return pos.x > cubeBounds.x_max || pos.x < cubeBounds.x_min;
	}

	bool outOfYBounds() {
		return pos.y > cubeBounds.y_min;  // y_max is not of interest here since the snow can be arbitrary high
	}

	bool outOfZBounds() {
		return pos.z > cubeBounds.z_max || pos.z < cubeBounds.z_min;
	}

	bool outOfBounds() {
		return outOfXBounds() || outOfYBounds() || outOfZBounds();
	}

	bool operator<(Particle& that) {
		return this->camDist > that.camDist;  // sorted in reverse, i.e. far away particles rendered first
	}
};


class ParticleSystem {
private:
	float lifeTime = 51.0f;
	float spread = 1.3f;
	float deltaLifeTime = 0.01f;
	glm::vec4 particleColor = glm::vec4(255, 255, 255, 200);
	glm::vec3 G;
	glm::vec3 spawnPos;
	float speedFac;

public:
	ParticleSystem(Shader* shader, glm::vec3 gravity, glm::vec3 pos, float speed) {
		this->shader = shader;
		this->G = gravity;
		this->spawnPos = pos;
		this->speedFac = speed;
		initParticleContainer();
		initBuffers();
		bindBuffers();
	}


	void draw() {
		// first argument: atribute buffer id as in bind[XY]Buffer, second argument: rate of advancing the vertex attribs
		glVertexAttribDivisor(0, 0);  // particle centers: reuse the same 4 vertices --> 0
		glVertexAttribDivisor(1, 1);  // position (center): 1 per quad --> 1
		glVertexAttribDivisor(2, 1);  // color: 1 per quad --> 1

		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particleCnt);
	}


	void generateParticles() {
		for (int i = 0; i < this->numNewParticles; i++) {
			int particleIdx = findNextParticle();
			particleContainer[particleIdx].life = lifeTime;	 // lifetime in s
			glm::vec2 randomPos = glm::diskRand(22.0f);
			particleContainer[particleIdx].pos = this->spawnPos + glm::vec3(randomPos.x, 0.0f, randomPos.y);

			
			glm::vec3 maindir = glm::vec3(0.0f, 10.0f, 0.0f);
			// generate random directions (there are better ways)  in range (-1,1)
			glm::vec3 randomdir = glm::vec3(
				(rand() % 2000 - 1000.0f) / 1000.0f,  // (0 bis 1999) --> (-1000 bis 999) --> (-1 bis 0.999)
				(rand() % 2000 - 1000.0f) / 1000.0f,
				(rand() % 2000 - 1000.0f) / 1000.0f
			);

			particleContainer[particleIdx].speed = maindir * spread;

			setParticleColor(particleIdx, particleColor);
			
			particleContainer[particleIdx].size = (rand() % 1000) / 2000.0f + 0.1f;

		}
	}


	void simulate() {
		this->particleCnt = 0;
		for (int i = 0; i < maxParticles; i++) {
			Particle& p = particleContainer[i];

			if (p.life > 0.0f) {
				p.life -= deltaLifeTime;
					if (p.life > 0.0f) {
						// simulate simple physics
						p.speed += G * (float)deltaLifeTime * 0.01f + ( (rand() % 2000 - 1000.0f) / 1000.0f );
						p.pos += p.speed * speedFac * ((rand() % 4));

						// check if out ouf bounds
						if (p.outOfBounds()) {
							p.reset();
							continue;
						}

						p.camDist = glm::length2( p.pos - camera.pos);

						setBufferData(p);  // fill the data for the GPU buffer
					}
					else {
						p.camDist = -1;
					}
			}

			particleCnt++;
		}
	}


	void sortParticles() {
		std::sort(&particleContainer[0], &particleContainer[maxParticles]);
	}


	void update(float dt, glm::mat4 projection, glm::mat4 view) {
		this->updateParticlesPerFrame(dt);
		this->generateParticles();                // generate new rand particles values
		this->simulate();                         // simulate particles
		this->sortParticles();                    // sort particles
		this->updateBuffers();                    // update bufers
		this->setShader(projection, view);        // set shader values
		this->draw();                             // draw particles
	}


	void updateBuffers() {
		updateParticlePosBuffer();
		updateParticleColBuffer();
	}


	void updateParticlesPerFrame(float dt) {
		numNewParticles = (int)(dt * 4000.0);

		//if (numNewParticles > (int)(0.016f * 5000.0))
			//numNewParticles = (int)(0.016f * 5000.0);
	}


	void setShader(glm::mat4 projection, glm::mat4 view) {
		this->shader->use();
		this->shader->setVec3("camRight", camera.right);
		this->shader->setVec3("camUp", camera.up);
		this->shader->setMat4("projection", projection);
		this->shader->setMat4("view", view);
	}


	void setParticleColor(int idx, glm::vec4 rgba) {
		particleContainer[idx].r = rgba.x;
		particleContainer[idx].g = rgba.y;
		particleContainer[idx].b = rgba.z;
		particleContainer[idx].a = rgba.w;
	}


private:
	Shader* shader;
	const static unsigned int maxParticles = MAX_PARTICLES;
	const GLfloat g_vertex_buffer_data[12] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	-0.5f,  0.5f, 0.0f,
	 0.5f,  0.5f, 0.0f,
	};
	GLfloat* g_particle_pos_size_data = new GLfloat[maxParticles * 4];
	GLubyte* g_particle_col_data = new GLubyte[maxParticles * 4];

	GLuint vertexBuffer;
	GLuint particlePosBuffer;
	GLuint particleColBuffer;
	unsigned int particleCnt = 0;
	Particle particleContainer[maxParticles];
	unsigned int lastUsedParticleIdx = 0;
	unsigned int numNewParticles = 0;


	void initParticleContainer() {
		for (int i = 0; i < maxParticles; i++) {
			this->particleContainer[i].life = -1.0f;
			this->particleContainer[i].camDist = -1.0f;
		}
	}


	void initBuffers() {
		initVerticesBuffer();
		initParticlePosBuffer();
		initParticleColBuffer();
	}


	void initVerticesBuffer() {
		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	}


	void initParticlePosBuffer() {
		glGenBuffers(1, &particlePosBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, particlePosBuffer);
		glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);  // empty at first, updated and filled each frame
	}


	void initParticleColBuffer() {
		glGenBuffers(1, &particleColBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, particleColBuffer);
		glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);  // empty at first, updated and filled each frame
	}


	void bindBuffers() {
		bindVerticesBuffer();
		bindParticlePosBuffer();
		bindParticleColBuffer();
	}


	void bindVerticesBuffer() { // positions of particles centers
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(
			0,			// attribute at position as specified in the shader
			3,			// size of element
			GL_FLOAT,	// type of element
			GL_FALSE,	// are elements normalized
			0,			// stride
			(void*)0	// offset
		);
	}


	void bindParticlePosBuffer() { // positions of particles centers
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, particlePosBuffer);
		glVertexAttribPointer(
			1,			// attribute at position as specified in the shader
			4,			// size of element
			GL_FLOAT,	// type of element
			GL_FALSE,	// are elements normalized
			0,			// stride
			(void*)0	// offset
		);
	}


	void bindParticleColBuffer() { // positions of particles centers
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, particleColBuffer);
		glVertexAttribPointer(
			2,					// attribute at position as specified in the shader
			4,					// size of element
			GL_UNSIGNED_BYTE,	// type of element
			GL_TRUE,			// are elements normalized
			0,					// stride
			(void*)0			// offset
		);
	}


	void updateParticlePosBuffer() {
		glBindBuffer(GL_ARRAY_BUFFER, particlePosBuffer);
		glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, particleCnt * 4 * sizeof(GLfloat), g_particle_pos_size_data);
	}


	void updateParticleColBuffer() {
		glBindBuffer(GL_ARRAY_BUFFER, particleColBuffer);
		glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, particleCnt * 4 * sizeof(GLubyte), g_particle_col_data);
	}


	int findNextParticle() {
		// search from lstUsedParticle to the end of the particle container
		for (int i = lastUsedParticleIdx; i < maxParticles; i++) {
			if (particleContainer[i].life < 0) {
				lastUsedParticleIdx = i;
				return i;
			}
		}

		// if not present search from beginning to the last used particle, i.e. all the rest
		for (int i = 0; i < lastUsedParticleIdx; i++) {
			if (particleContainer[i].life < 0) {
				lastUsedParticleIdx = i;
				return i;
			}
		}

		return 0; // if all particles used, return zero
	}


	int getParticlesPerFrame() {
		return this->numNewParticles;
	}


	void setBufferData(Particle& p) {
		g_particle_pos_size_data[4 * particleCnt + 0] = p.pos.x;
		g_particle_pos_size_data[4 * particleCnt + 1] = p.pos.y;
		g_particle_pos_size_data[4 * particleCnt + 2] = p.pos.z;
		g_particle_pos_size_data[4 * particleCnt + 3] = p.size;

		g_particle_col_data[4 * particleCnt + 0] = p.r;
		g_particle_col_data[4 * particleCnt + 1] = p.g;
		g_particle_col_data[4 * particleCnt + 2] = p.b;
		g_particle_col_data[4 * particleCnt + 3] = p.a;
	}
};

#endif // !PARTICLESYSTEM_H
