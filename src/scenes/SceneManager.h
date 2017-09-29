#pragma once

#include "Scene.h"
#include <map>
#include <memory>
#include <GLFW/glfw3.h>

class SceneManager
{
private:
    std::map<std::string, std::unique_ptr<Scene>> scenes_;
    Scene* activeScene_;
    static SceneManager* instance_;
    SceneManager();

public:
    ~SceneManager();
    static std::unique_ptr<SceneManager> createInstance();
    static SceneManager* getInstance();
    void activateScene(std::string sceneName);
    Scene* getActiveScene();
    void loadSceneFromSqliteDb(std::string sceneName);

};