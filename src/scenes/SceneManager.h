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

    GLFWwindow * window_;

public:
    SceneManager(GLFWwindow * window);
    void activateScene(std::string sceneName);
    Scene& getActiveScene() const;
    void loadSceneFromSqliteDb(std::string sceneName);

};