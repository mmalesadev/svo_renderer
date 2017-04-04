#include "SceneManager.h"

void SceneManager::loadScene(std::string sceneName)
{
	auto scene = scenes.find(sceneName);
	if (scene == scenes.end())
	{
		scenes[sceneName] = std::make_unique<Scene>();
	}
}

void SceneManager::activateScene(std::string sceneName)
{
	auto scene = scenes.find(sceneName);
	if(scene != scenes.end())
	{
		activeScene = scene->second.get();
	}
	else
	{
		loadScene(sceneName);
	}
}

