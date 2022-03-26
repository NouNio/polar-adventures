/*
* Copyright 2020 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/

#pragma once

#include "general/opengl.h"
#include "general/controls.h"
#include "Light.h"
#include "GameCamera.h"











bool addedReflection = false;


std::vector<glm::vec3> lightPositions;


/* --------------------------------------------- */
// Prototypes
/* --------------------------------------------- */

void setPerFrameUniforms(GameShader* shader, GameCamera& camera, DirectionalLight& dirL)
{
	shader->use();
	shader->setUniform("viewProjMatrix", camera.getCombined());
	shader->setUniform("camera_world", camera.getPosition());
	shader->setUniform("dirL.color", dirL.color);
	shader->setUniform("dirL.direction", dirL.direction);
}

/* --------------------------------------------- */
// Global variables
/* --------------------------------------------- */








/* --------------------------------------------- */
// Main
/* --------------------------------------------- */

int main(int argc, char** argv)
{
	/* --------------------------------------------- */
	// Load settings.ini
	/* --------------------------------------------- */

	INIReader reader("assets/settings.ini");
	int window_width = reader.GetInteger("window", "width", 800);
	int window_height = reader.GetInteger("window", "height", 800);
	int refresh_rate = reader.GetInteger("window", "refresh_rate", 60);
	fullscreen = reader.GetBoolean("window", "fullscreen", false);
	std::string window_title = reader.Get("window", "title", "ECG 2020");
	float brightness = float(reader.GetReal("window", "brightness", 1.0f));
	float fov = float(reader.GetReal("camera", "fov", 60.0f));
	float nearZ = float(reader.GetReal("camera", "near", 0.1f));
	float farZ = float(reader.GetReal("camera", "far", 100.0f));


	/* --------------------------------------------- */
	// Create context
	/* --------------------------------------------- */

	glfwSetErrorCallback([](int error, const char* description) { std::cout << "GLFW error " << error << ": " << description << std::endl; });

	if (!glfwInit()) {
		EXIT_WITH_ERROR("Failed to init GLFW");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // Request OpenGL version 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Request core profile
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);  // Create an OpenGL debug context 
	glfwWindowHint(GLFW_REFRESH_RATE, refresh_rate); // Set refresh rate
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// Enable antialiasing (4xMSAA)
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Open window
	monitor = nullptr;

	if (fullscreen)
		monitor = glfwGetPrimaryMonitor();
	window = glfwCreateWindow(window_width, window_height, window_title.c_str(), monitor, nullptr);
	if (!window) EXIT_WITH_ERROR("Failed to create window");


	// This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true;
	GLenum err = glewInit();

	// If GLEW wasn't initialized
	if (err != GLEW_OK) {
		EXIT_WITH_ERROR("Failed to init GLEW: " << glewGetErrorString(err));
	}

	// Debug callback
	if (glDebugMessageCallback != NULL) {
		// Register your callback function.

		glDebugMessageCallback(DebugCallbackDefault, NULL);
		// Enable synchronous callback. This ensures that your callback function is called
		// right after an error has occurred. This capability is not defined in the AMD
		// version.
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}


	/* --------------------------------------------- */
	// Init framework
	/* --------------------------------------------- */

	//if (!initFramework()) {EXIT_WITH_ERROR("Failed to init framework");}

	// set callbacks
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);

	// set GL defaults
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	

	//create framebuffers?
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	unsigned int blur[2];
	glGenFramebuffers(2, blur);
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	unsigned int colorBuffers[2];
	glGenTextures(2, colorBuffers);
	unsigned int blurText;
	glGenTextures(1, &blurText);
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	initializeFBOs(window_width,window_height,fbo, blur, colorBuffers, attachments, rbo, blurText);
	unsigned int cubemap;
	glGenTextures(1, &cubemap);
	createCubeMap(cubemap);

	/* --------------------------------------------- */
	// Initialize scene and render loop
	/* --------------------------------------------- */

		// Load shader(s)


	std::shared_ptr<GameShader> shader = std::make_shared<GameShader>("texture.vert","texture.frag");
	std::shared_ptr<GameShader> screenShader=std::make_shared<GameShader>("screen.vert","screen.frag");
	screenShader->use();
	glUniform1i(glGetUniformLocation(screenShader->_handle, "bloom"), 1);
	screenShader->setUniform("brightness", brightness);
	screenShader->unuse();

	std::shared_ptr<GameShader> blurShader(std::make_shared<GameShader>("screen.vert", "blur.frag"));
	std::shared_ptr<GameShader> envShader(std::make_shared<GameShader>("environment.vert", "environment.frag"));
	envShader->use();
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	glUniform1i(glGetUniformLocation(envShader->_handle, "environment"), 2);
	std::shared_ptr<GameShader> mirrorShader(std::make_shared<GameShader>("mirror.vert", "environment.frag"));
	mirrorShader->use();
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	glUniform1i(glGetUniformLocation(mirrorShader->_handle, "environment"), 2);
	mirrorShader->unuse();

	std::shared_ptr<GameShader> textShader(std::make_shared<GameShader>("text.vert", "text.frag"));
	glm::mat4 textProj = glm::ortho(0.0f, static_cast<float>(window_width), 0.0f, static_cast<float>(window_height));
	textShader->use();
	textShader->setUniform("brightness", brightness);
	glUniformMatrix4fv(glGetUniformLocation(textShader->_handle, "projection"), 1, GL_FALSE, glm::value_ptr(textProj));

	


	float worldsize = 100;
	glm::vec3 p1(-worldsize, 0, worldsize);//bottom front left
	glm::vec3 p2(worldsize, 0, worldsize);//bottom front right
	glm::vec3 p3(worldsize, 0, -worldsize);//bottom back right
	glm::vec3 p4(-worldsize, 0, -worldsize);//bottom back left
	//create frameBuffer plane
	glm::vec3 p5(-1, 1, 0);//top left
	glm::vec3 p6(1, 1, 0);//top right
	glm::vec3 p7(1, -1, 0);//bottom right
	glm::vec3 p8(-1, -1, 0);//bottom left
	// Create textures
	std::shared_ptr<TwoDTexture> woodTexture = std::make_shared<TwoDTexture>("wood_texture.dds");
	std::shared_ptr<TwoDTexture> tileTexture = std::make_shared<TwoDTexture>("tiles_diffuse.dds");
	std::shared_ptr<TwoDTexture> blank = std::make_shared<TwoDTexture>("blank.dds");
	std::shared_ptr<TwoDTexture> ground = std::make_shared<TwoDTexture>("world ground.dds");
	// Create materials
	std::shared_ptr<TextureMaterial> woodTextureMaterial = std::make_shared<TextureMaterial>(shader, glm::vec3(0.1f, 0.7f, 0.1f), 2.0f, woodTexture);
	std::shared_ptr<TextureMaterial> tileTextureMaterial = std::make_shared<TextureMaterial>(shader, glm::vec3(0.1f, 0.7f, 0.3f), 8.0f, tileTexture);
	std::shared_ptr<TextureMaterial> beamMaterial = std::make_shared <TextureMaterial>(shader, glm::vec3(0.5f, 0.0f, 0.0f), 2.0f, blank);
	std::shared_ptr<TextureMaterial> floorMaterial = std::make_shared <TextureMaterial>(shader, glm::vec3(0.99f, 0.01f, 0.3f), 2.0f, ground);



	Geometry floor = Geometry(glm::mat4(1), Geometry::createPane(p4, p3, p2, p1), floorMaterial);
	//todo exchange to pointer
	

	Geometry screen = Geometry(glm::mat4(1), Geometry::createPane(p7, p8,p5,p6), std::make_shared<TextureMaterial>(screenShader, glm::vec3(1.0f, 0.0f, 0.0f), 1.0f, std::make_shared<TwoDTexture>(colorBuffers[0])));
	Geometry world = Geometry(glm::mat4(1), Geometry::createCubeGeometry(2.0f, 2.0f, 2.0f), std::make_shared<TextureMaterial>(envShader, glm::vec3(1.0f, 0.0f, 0.0f), 1.0f, std::make_shared<TwoDTexture>(colorBuffers[0])));
	glm::vec3  endpoint(15, 12, 13);
	Geometry endedge=Geometry(glm::translate(glm::mat4(1),endpoint), Geometry::readModel("ziel rahmen"), woodTextureMaterial);
	Geometry end = Geometry(glm::translate(glm::mat4(1), endpoint), Geometry::createSphereGeometry(14,7,1), tileTextureMaterial);
	glm::vec3 mp1(-1, 0, 1); glm::vec3 mp2(1, 0, 1); glm::vec3 mp3(1, 0, -1); glm::vec3 mp4(-1, 0, -1);
	Geometry frame(glm::mat4(1.0), Geometry::readModel("mirror rahmen"), woodTextureMaterial);
	/*PxShape* shape = gPhysics->createShape(PxSphereGeometry(1.0f),PxMaterial(woodTextureMaterial));
	myActor.attachShape(*shape);
	shape->release();*/
	world.setShader(envShader);
		// Initialize camera
	std::shared_ptr<GameCamera>  renderCamera=std::make_shared<GameCamera>(glm::vec3(0, 2, -10), 0, 0, fov, float(window_height), float(window_width), nearZ, farZ, false);
	std::shared_ptr<GameCamera>  playerCamera= std::make_shared<GameCamera>(glm::vec3(0, 2, -10), 0, 0, fov, float(window_height), float(window_width), nearZ, farZ, true);
	std::shared_ptr<GameCamera>  camera=renderCamera;
		// Initialize lights
		DirectionalLight dirL(glm::vec3(0.8f), glm::vec3(0.0f, -1.0f, -1.0f));

		//Deleting this apparently makes the whole thing go boom i suppose its the shader, ill check again
			Geometry light2 = Geometry(glm::mat4(1.0), Geometry::createCylinderGeometry(10, glm::distance(glm::vec3(), glm::vec3(0, 7.0f, -5.5f)), 0.1), beamMaterial); 

			// Render loop

		double last = glfwGetTime();
		float t = float(last);
		float dt = 0.0f;
		float t_sum = 0.0f;
		double mouse_x, mouse_y;
		positionChange = glm::vec3();
		

		
		while (!glfwWindowShouldClose(window)) {
			if (!debugCamera) {
				camera = playerCamera;
			}
			else {
				camera = renderCamera;
			}
			u_d = 0;
			l_r = 0;
			// Clear backbuffer
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			{glFrontFace(GL_CCW);
			glDepthMask(GL_FALSE);
			envShader->use();
			envShader->setUniform("viewProjMatrix", camera->getEnvMatrix());
			world.draw();
			glDepthMask(GL_TRUE);

			glClear(GL_DEPTH_BUFFER_BIT);
			glFrontFace(GL_CW);
			glEnable(GL_DEPTH_TEST); }
		
			// Poll events
			glfwPollEvents();
			//make sure that that doesn't go boom
			//if (dt == 0.0f) { dt = 1.0; }
			updateMovement(16 * dt);


			// Update camera
			if (true)
			{
				glfwGetCursorPos(window, &mouse_x, &mouse_y);
				camera->updateRotation(float(mouse_y), float(-mouse_x));
				camera->moveCamera(positionChange);
				
			
			}
			// Set per-frame uniforms
			setPerFrameUniforms(shader.get(), *camera, dirL);
			
			// Render
			//cuboidBody->setKinematicTarget(PxTransform(PxVec3(cuboids[0]->getPosition().x, cuboids[0]->getPosition().y, cuboids[0]->getPosition().z)));

			mirrorShader->use();
			mirrorShader->setUniform("viewProjMatrix", camera->getCombined());
			envShader->setUniform("viewDir", camera->getViewVector());
	
	
	
//		
			//if(currentLevel>1)cuboidShort.draw();
			floor.draw();
			//draw lights
			{	shader->use();
			shader->setUniform("glowing", true);
			//lineL2.draw();
			
				//lineInv.draw();
			shader->use();
			shader->setUniform("glowing", false);
		}
			//draw endpoint
			{end.draw();
			shader->use();
			shader->setUniform("externalrotation", 32 * t_sum);
			shader->unuse();
			endedge.draw();
			shader->use();
			shader->setUniform("externalrotation", 0.0f); }


			//blurring loop
			for (size_t i = 0; i < 2; i++)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, blur[0]);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glDisable(GL_DEPTH_TEST);
				screen.setShader(blurShader);
				screen.setTexture(std::make_shared<TwoDTexture>(colorBuffers[1]));
				blurShader->use();
				blurShader->setUniform("vertical", true);

				screen.draw();
				glBindFramebuffer(GL_FRAMEBUFFER, blur[1]);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glDisable(GL_DEPTH_TEST);
				screen.setTexture(std::make_shared<TwoDTexture>(blurText));
				blurShader->setUniform("vertical", false);
				screen.draw();
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
			glDisable(GL_DEPTH_TEST);

			
			
			screen.setShader(screenShader);
			screen.setTexture(std::make_shared<TwoDTexture>(colorBuffers[0]));
			screenShader->use();
			
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, colorBuffers[1]);
	
			screen.draw();
			//renderText(textShader, "Test", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));


			positionChange = glm::vec3();
			// Compute frame time
			dt = t;
			t = float(glfwGetTime());
			dt = t - dt;
			t_sum += dt;
			
			// Swap buffers
			glfwSwapBuffers(window);
		}
	


	/* --------------------------------------------- */
	// Destroy framework
	/* --------------------------------------------- */
	glDeleteFramebuffers(1, &fbo);
	//destroyFramework();


	/* --------------------------------------------- */
	// Destroy context and exit
	/* --------------------------------------------- */

	glfwTerminate();

	return EXIT_SUCCESS;
}




