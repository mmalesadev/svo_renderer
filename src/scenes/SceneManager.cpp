#include "SceneManager.h"

void SceneManager::loadScene(std::string sceneName)
{
	auto scene = scenes_.find(sceneName);
	if (scene == scenes_.end())
	{
		scenes_[sceneName] = std::make_unique<Scene>();
	}
}

void SceneManager::activateScene(std::string sceneName)
{
	auto scene = scenes_.find(sceneName);
	if(scene != scenes_.end())
	{
		activeScene_ = scene->second.get();
	}
	else
	{
		loadScene(sceneName);
	}
}

