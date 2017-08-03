#pragma once
#include "Scene.h"
#include <map>
#include <memory>

class SceneManager
{
private:
	std::map<std::string, std::unique_ptr<Scene>> scenes_;
	Scene* activeScene_;

public:
	void activateScene(std::string sceneName);

	Scene& getActiveScene() const { return *activeScene_; }
	
	void loadSceneFromSqliteDb(std::string sceneName);

};