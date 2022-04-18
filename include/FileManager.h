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


	string getAudioPath(string audioKey) {
		return this->pathToProject + ASSETS + AUDIO + this->pathToAudio[audioKey];
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
	const string AUDIO = "audio\\";
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
	map<string, string> pathToAudio = map_list_of("background1", "8-Bit-Ninja.mp3")
												 ("background2", "summer-adventure-dj-soul.mp3")
		                                         ("walk1", "sfx_movement_footsteps1a.wav")
		                                         ("walk2", "sfx_movement_footsteps1b.wav")
												 ("jump", "sfx_movement_jump1.wav")
												 ("pickUp", "sfx_sounds_fanfare3.wav")
											     ("deliver", "sfx_sounds_powerup16.wav")
												 ("win", "Jingle_Win_00.mp3")
												 ("lose", "Jingle_Lose_00.mp3")
											     ("alarm", "sfx_alarm_loop1.wav");
};  

#endif // !FILEMANAGER_H
