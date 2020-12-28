#pragma once

#include "System.h"
#include "SceneManager.h"
#include <unordered_map>
#include <vector>
#include <spdlog/spdlog.h>

class InputSystem : public System
{
public:
    InputSystem();
    virtual void update();

    std::vector< std::pair<std::string, std::string> >& getActionList() { return actionList_; }

private:
    enum class InputMode { FREE_ROAM_MODE, GUI_MODE };
    enum class MovementType { MOVE_FORWARD, MOVE_BACKWARD, MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN, ROLL_LEFT, ROLL_RIGHT};

    void moveActiveCamera(MovementType movementType);
    void toggleFreeRoam();
    void toggleGui();

    void prepareGlfwKeyMaps();
    void loadActionKeyMapFromSqliteDb();
    void addActionKey(std::string action, std::string key);

    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    std::unordered_map<std::string, std::string> actionKeyMap_;
    std::unordered_map<std::string, std::string> actionKeyMapReverse_;
    typedef std::unordered_map<InputMode, std::unordered_map<std::string, std::function<void(void)> > > InputModeActionMap;
    InputModeActionMap actionMap_;
    InputModeActionMap keyCallbackActionMap_;
    std::unordered_map<std::string, int> glfwKeyMap_;
    std::unordered_map<int, std::string> glfwKeyMapReverse_;

    std::unordered_map<std::string, std::string> actionLabels_;

    std::vector< std::pair<std::string, std::string> > actionList_;

    InputMode inputMode_;
};