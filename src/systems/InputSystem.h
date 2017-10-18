#pragma once

#include "System.h"

class InputSystem : public System
{
public:
    InputSystem();
    virtual void update();

private:
    enum class MovementType { MOVE_FORWARD, MOVE_BACKWARD, MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN, ROLL_LEFT, ROLL_RIGHT};
    void moveActiveCamera(MovementType movementType);

    void loadActionKeyMapFromSqliteDb();
    void addActionKey(std::string action, std::string key);

    std::map<std::string, std::string> keyActionMap_;
    std::map<std::string, std::function<void(void)>> actionMap_;
    std::map<std::string, int> glfwKeyMap_;
};