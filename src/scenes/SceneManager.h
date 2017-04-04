#pragma once
#include "Scene.h"
#include <map>
#include <memory>

class SceneManager
{
private:
	std::map<std::string, std::unique_ptr<Scene>> scenes;
	Scene* activeScene;

public:
	void loadScene(std::string sceneName);
	void activateScene(std::string sceneName);

	Scene* getActiveScene() const { return activeScene; }
};