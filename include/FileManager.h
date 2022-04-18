#pragma once
#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <map>
#include <boost/assign.hpp>
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;
using namespace boost::assign;

class FileManager {
public:
	FileManager()
	{
		pathToProject = fs::current_path().string();
	}


	string getFontPath(string fontKey)
	{
		return this->pathToProject + ASSETS + FONTS + this->pathToFont[fontKey];
	}


	string getIniPath()
	{
		return this->pathToProject + ASSETS + INI;
	}


	string getObjPath(string objKey)
	{
		return this->pathToProject + ASSETS + OBJECTS + this->pathToObject[objKey];
	}


	string getShaderPath(string shaderKey)
	{	
		return this->pathToProject + ASSETS + SHADER + pathToShader[shaderKey];
	}


	string getProjectPath()
	{
		return pathToProject;
	}


private:
	std::string pathToProject;
	const string INI = "settings.ini";
	const string ASSETS = "\\assets\\";
	const string FONTS = "fonts\\";
	const string OBJECTS = "objects\\";
	const string SHADER = "shader\\";
	
	const char* fontPath;
	map<string, string> pathToObject = map_list_of("player", "kenny-block-characters\\player.fbx")
												  ("snowball", "snowball\\snowball.fbx")
		                                          ("obelisk", "kenny-nature-kit\\statue_obelisk.fbx")
												  ("test-world", "world\\test-world.fbx");
	map<string, string> pathToShader = map_list_of	("modelVert", "model.vert") ("modelFrag", "model.frag")
													("playerVert", "player.vert") ("playerFrag", "player.frag")
													("directLightVert", "directLight.vert") ("directLightFrag", "directLight.frag")
													("HUDvert", "HUD.vert") ("HUDfrag", "HUD.frag");
	map<string, string> pathToFont = map_list_of	("datcub", "datcub\\datcub.ttf")
													("arial", "arial\\arial.ttf");
};  

#endif // !FILEMANAGER_H
