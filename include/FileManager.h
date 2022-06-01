#pragma once
#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <map>
#include <filesystem>


class FileManager {
public:
	FileManager()
	{
		pathToProject = std::filesystem::current_path().string();
	}


	std::string getAudioPath(std::string audioKey) {
		return this->pathToProject + ASSETS + AUDIO + this->pathToAudio[audioKey];
	}


	std::string getFontPath(std::string fontKey)
	{
		return this->pathToProject + ASSETS + FONTS + this->pathToFont[fontKey];
	}


	std::string getIniPath()
	{
		return this->pathToProject + ASSETS + INI;
	}


	std::string getObjPath(std::string objKey)
	{
		return this->pathToProject + ASSETS + OBJECTS + this->pathToObject[objKey];
	}


	std::string getShaderPath(std::string shaderKey, bool isKnownPath=false)
	{	
		if (isKnownPath) return  this->pathToProject + ASSETS + SHADER + shaderKey;
	else
		return this->pathToProject + ASSETS + SHADER + pathToShader[shaderKey];
	}


	std::string getSkyBoxPath(std::string skyboxKey) {
		return this->pathToProject + ASSETS + SKYBOX + pathToSkybox[skyboxKey];
	}


	std::string getPlayerPath(std::string playerKey) {
		return this->pathToProject + ASSETS + PLAYER + pathToObject[playerKey];
	}


	std::string getProjectPath()
	{
		return pathToProject;
	}


	std::string getWorldPath(std::string worldKey) {
		return  this->pathToProject + ASSETS + WORLD + pathToObject[worldKey];
	}


private:
	std::string pathToProject;
	const std::string INI = "settings.ini";
	const std::string ASSETS = "\\assets\\";
	const std::string AUDIO = "audio\\";
	const std::string FONTS = "fonts\\";
	const std::string OBJECTS = "objects\\";
	const std::string PLAYER = "player\\";
	const std::string SHADER = "shader\\";
	const std::string SKYBOX = "skybox\\";
	const std::string WORLD = "world\\";
	
	const char* fontPath;
	std::map<std::string, std::string> pathToObject = { {"player", "losstronaut.dae"},
														{"snowball", "snowball\\snowball.fbx"},
														{"snowman", "kenny-holiday-kit\\snowmanFancy.fbx"},
														{"perm-wall", "kenny-nature-kit\\cliff_block_stone.fbx"},
														{"game-world", "game-world.fbx"}, 
														{"game-world-centered", "game-world-centered.fbx"}, };
	
	std::map<std::string, std::string> pathToShader = { 
		
		{"directLightVert", "directLight.vert"},
		{"directLightFrag", "directLight.frag"},
		{"HUDvert", "HUD.vert"},
		{"HUDfrag", "HUD.frag"},
		{"modelVert", "model.vert"},
		{"modelFrag", "model.frag"},
		{"animModelVert", "animModel.vert"},
		{"animModelFrag", "animModel.frag"},
		{"pass_on", "pass_on.vert" },
	    {"idleVert", "idleModel.vert"},
		{"idleFrag", "idleModel.frag"},
		{"particleVert", "particle.vert"},
		{"particleFrag", "particle.frag"},
		{"sobel", "sobel.frag"},
	    {"screen","screen.frag"},
		{"skyboxVert", "skybox.vert"}, 
		{"skyboxFrag", "skybox.frag" }
		
													
	};
	
	std::map<std::string, std::string> pathToFont = { {"arial", "arial\\arial.ttf"} };
	
	std::map<std::string, std::string> pathToAudio = { {"background1", "8-Bit-Ninja.mp3"},
													   {"walk1", "sfx_movement_footsteps1a.wav"},
													   {"walk2", "sfx_movement_footsteps1b.wav"},
													   {"throw", "sfx_weapon_singleshot1.wav"},
													   {"noAmmo", "sfx_wpn_noammo3.wav"},
													   {"jump", "sfx_movement_jump1.wav"},
													   {"pickUp", "sfx_sounds_fanfare3.wav"},
													   {"deliver", "sfx_sounds_powerup16.wav"},
													   {"alarm", "sfx_alarm_loop1.wav"},
													   {"win", "Jingle_Win_00.mp3"},
													   {"lose", "Jingle_Lose_00.mp3"} };
	
	std::map<std::string, std::string> pathToSkybox = {{"daylight", "daylight\\"}, {"galaxy", "galaxy\\"}, {"orange", "orange\\"},};
};  

#endif // !FILEMANAGER_H
