#include "InputSystem.h"
#include "ProgramVariables.h"
#include <functional>
#include <fstream>
#include <iostream>
#include <json.h>

InputSystem::InputSystem() : inputMode_(InputMode::GUI_MODE)
{
    // Possible actions are defined here
    actionMap_[InputMode::FREE_ROAM_MODE]["MOVE_CAMERA_FORWARD"] = std::bind(&InputSystem::moveActiveCamera, this, MovementType::MOVE_FORWARD);
    actionMap_[InputMode::FREE_ROAM_MODE]["MOVE_CAMERA_BACKWARD"] = std::bind(&InputSystem::moveActiveCamera, this, MovementType::MOVE_BACKWARD);;
    actionMap_[InputMode::FREE_ROAM_MODE]["MOVE_CAMERA_LEFT"] = std::bind(&InputSystem::moveActiveCamera, this, MovementType::MOVE_LEFT);
    actionMap_[InputMode::FREE_ROAM_MODE]["MOVE_CAMERA_RIGHT"] = std::bind(&InputSystem::moveActiveCamera, this, MovementType::MOVE_RIGHT);
    actionMap_[InputMode::FREE_ROAM_MODE]["ROLL_CAMERA_LEFT"] = std::bind(&InputSystem::moveActiveCamera, this, MovementType::ROLL_LEFT);
    actionMap_[InputMode::FREE_ROAM_MODE]["ROLL_CAMERA_RIGHT"] = std::bind(&InputSystem::moveActiveCamera, this, MovementType::ROLL_RIGHT);
    actionMap_[InputMode::FREE_ROAM_MODE]["MOVE_CAMERA_UP"] = std::bind(&InputSystem::moveActiveCamera, this, MovementType::MOVE_UP);
    actionMap_[InputMode::FREE_ROAM_MODE]["MOVE_CAMERA_DOWN"] = std::bind(&InputSystem::moveActiveCamera, this, MovementType::MOVE_DOWN);

    keyCallbackActionMap_[InputMode::FREE_ROAM_MODE]["TOGGLE_FREE_ROAM"] = std::bind(&InputSystem::toggleFreeRoam, this);
    keyCallbackActionMap_[InputMode::GUI_MODE]["TOGGLE_FREE_ROAM"] = std::bind(&InputSystem::toggleFreeRoam, this);

    keyCallbackActionMap_[InputMode::FREE_ROAM_MODE]["TOGGLE_GUI"] = std::bind(&InputSystem::toggleGui, this);
    keyCallbackActionMap_[InputMode::GUI_MODE]["TOGGLE_GUI"] = std::bind(&InputSystem::toggleGui, this);
    
    loadActionKeyMapFromJsonFile();

    prepareGlfwKeyMaps();

    auto keyCallback = [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto me = (InputSystem*)glfwGetWindowUserPointer(window);
        me->keyCallback(window, key, scancode, action, mods);
    };
    glfwSetWindowUserPointer(ProgramVariables::getWindow(), this);
    glfwSetKeyCallback(ProgramVariables::getWindow(), keyCallback);

    if (inputMode_ == InputMode::FREE_ROAM_MODE) glfwSetInputMode(ProgramVariables::getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    actionLabels_["MOVE_CAMERA_FORWARD"] = "Forward";
    actionLabels_["MOVE_CAMERA_BACKWARD"] = "Backward";
    actionLabels_["MOVE_CAMERA_LEFT"] = "Left";
    actionLabels_["MOVE_CAMERA_RIGHT"] = "Right";
    actionLabels_["ROLL_CAMERA_LEFT"] = "Roll left";
    actionLabels_["ROLL_CAMERA_RIGHT"] = "Roll right";
    actionLabels_["MOVE_CAMERA_UP"] = "Up";
    actionLabels_["MOVE_CAMERA_DOWN"] = "Down";
    actionLabels_["TOGGLE_FREE_ROAM"] = "Toggle free roam";
    actionLabels_["TOGGLE_GUI"] = "Hide/show GUI";

    auto addActionToActionsInfoList = [this](std::string action) -> void
    {
        actionList_.push_back({ actionLabels_[action], actionKeyMap_[action] });
    };

    addActionToActionsInfoList("MOVE_CAMERA_FORWARD");
    addActionToActionsInfoList("MOVE_CAMERA_BACKWARD");
    addActionToActionsInfoList("MOVE_CAMERA_LEFT");
    addActionToActionsInfoList("MOVE_CAMERA_RIGHT");
    addActionToActionsInfoList("MOVE_CAMERA_UP");
    addActionToActionsInfoList("MOVE_CAMERA_DOWN");
    addActionToActionsInfoList("ROLL_CAMERA_LEFT");
    addActionToActionsInfoList("ROLL_CAMERA_RIGHT");
    addActionToActionsInfoList("TOGGLE_FREE_ROAM");
    addActionToActionsInfoList("TOGGLE_GUI");
}

void InputSystem::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        auto& actionFunc = keyCallbackActionMap_[inputMode_][actionKeyMapReverse_[glfwKeyMapReverse_[key]]];
        if (actionFunc) actionFunc();
    }
}

void InputSystem::update()
{
    // Check all possible actions for this mode
    for (auto action : actionMap_[inputMode_])
    {
        if (glfwGetKey(ProgramVariables::getWindow(), glfwKeyMap_[actionKeyMap_[action.first]]) == GLFW_PRESS)
        {
            auto& actionFunc = action.second;
            if (actionFunc) actionFunc();
        }
    }

    // If it's GUI_MODE, we dont do the mouse camera movement
    if (inputMode_ == InputMode::GUI_MODE)
        return;

    // Mouse movement - rotating camera 
    double xPos, yPos;
    int width, height;

    glfwGetCursorPos(ProgramVariables::getWindow(), &xPos, &yPos);
    glfwGetWindowSize(ProgramVariables::getWindow(), &width, &height);
    glfwSetCursorPos(ProgramVariables::getWindow(), width / 2, height / 2);
    auto activeCamera = SceneManager::getInstance()->getActiveCamera();
    if (activeCamera)
    {
        auto& transformComponent = SceneManager::getInstance()->getComponent<TransformComponent>(*activeCamera, TRANSFORM_COMPONENT_ID);
        auto& cameraComponent = SceneManager::getInstance()->getComponent<CameraComponent>(*activeCamera, CAMERA_COMPONENT_ID);
        glm::quat deltaPitch = glm::angleAxis(0.002f * float(height / 2 - yPos), cameraComponent.getRight());
        glm::quat deltaYaw = glm::angleAxis(0.002f * float(width / 2 - xPos), cameraComponent.getUp());
        transformComponent.setQuaternion(glm::normalize(deltaPitch * deltaYaw * transformComponent.getQuaternion()));
        cameraComponent.update(transformComponent);
    }
}

void InputSystem::moveActiveCamera(MovementType movementType)
{
    auto activeCamera = SceneManager::getInstance()->getActiveCamera();
    if (activeCamera)
    {
        auto& transformComponent = SceneManager::getInstance()->getComponent<TransformComponent>(*activeCamera, TRANSFORM_COMPONENT_ID);
        auto& cameraComponent = SceneManager::getInstance()->getComponent<CameraComponent>(*activeCamera, CAMERA_COMPONENT_ID);

        switch (movementType)
        {
        case MovementType::MOVE_FORWARD:
            transformComponent.setPosition(transformComponent.getPosition() + cameraComponent.getDirection() * ProgramVariables::getDeltaTime() * cameraComponent.getSpeed());
            break;
        case MovementType::MOVE_BACKWARD:
            transformComponent.setPosition(transformComponent.getPosition() - cameraComponent.getDirection() * ProgramVariables::getDeltaTime() * cameraComponent.getSpeed());
            break;
        case MovementType::MOVE_LEFT:
            transformComponent.setPosition(transformComponent.getPosition() - cameraComponent.getRight() * ProgramVariables::getDeltaTime() * cameraComponent.getSpeed());
            break;
        case MovementType::MOVE_RIGHT:
            transformComponent.setPosition(transformComponent.getPosition() + cameraComponent.getRight() * ProgramVariables::getDeltaTime() * cameraComponent.getSpeed());
            break;
        case MovementType::ROLL_LEFT:
        {
            glm::quat deltaRoll = glm::angleAxis(-1.0f * ProgramVariables::getDeltaTime(), cameraComponent.getDirection());
            transformComponent.setQuaternion(glm::normalize(deltaRoll * transformComponent.getQuaternion()));
            break;
        }
        case MovementType::ROLL_RIGHT:
        {
            glm::quat deltaRoll = glm::angleAxis(1.0f * ProgramVariables::getDeltaTime(), cameraComponent.getDirection());
            transformComponent.setQuaternion(glm::normalize(deltaRoll * transformComponent.getQuaternion()));
            break;
        }
        case MovementType::MOVE_UP:
            transformComponent.setPosition(transformComponent.getPosition() + ProgramVariables::getDeltaTime() * cameraComponent.getSpeed() * cameraComponent.getUp());
            break;
        case MovementType::MOVE_DOWN:
            transformComponent.setPosition(transformComponent.getPosition() - ProgramVariables::getDeltaTime() * cameraComponent.getSpeed() * cameraComponent.getUp());
            break;
        default:
            break;
        }

        cameraComponent.update(transformComponent);
    }
}

void InputSystem::toggleFreeRoam()
{
    if (inputMode_ == InputMode::FREE_ROAM_MODE)
    {
        inputMode_ = InputMode::GUI_MODE;
        glfwSetInputMode(ProgramVariables::getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    else if (inputMode_ == InputMode::GUI_MODE)
    {
        inputMode_ = InputMode::FREE_ROAM_MODE;
        int width, height;
        glfwGetWindowSize(ProgramVariables::getWindow(), &width, &height);
        glfwSetCursorPos(ProgramVariables::getWindow(), width / 2, height / 2);
        glfwSetInputMode(ProgramVariables::getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

void InputSystem::toggleGui()
{
    ProgramVariables::toggleGui();
}

void InputSystem::prepareGlfwKeyMaps()
{
    // Possible keys used in program are defined here:
    glfwKeyMap_["Q"] = GLFW_KEY_Q;
    glfwKeyMap_["W"] = GLFW_KEY_W;
    glfwKeyMap_["E"] = GLFW_KEY_E;
    glfwKeyMap_["R"] = GLFW_KEY_R;
    glfwKeyMap_["T"] = GLFW_KEY_T;
    glfwKeyMap_["Y"] = GLFW_KEY_Y;
    glfwKeyMap_["U"] = GLFW_KEY_U;
    glfwKeyMap_["I"] = GLFW_KEY_I;
    glfwKeyMap_["O"] = GLFW_KEY_O;
    glfwKeyMap_["P"] = GLFW_KEY_P;
    glfwKeyMap_["A"] = GLFW_KEY_A;
    glfwKeyMap_["S"] = GLFW_KEY_S;
    glfwKeyMap_["D"] = GLFW_KEY_D;
    glfwKeyMap_["F"] = GLFW_KEY_F;
    glfwKeyMap_["G"] = GLFW_KEY_G;
    glfwKeyMap_["H"] = GLFW_KEY_H;
    glfwKeyMap_["J"] = GLFW_KEY_J;
    glfwKeyMap_["K"] = GLFW_KEY_K;
    glfwKeyMap_["L"] = GLFW_KEY_L;
    glfwKeyMap_["Z"] = GLFW_KEY_Z;
    glfwKeyMap_["X"] = GLFW_KEY_X;
    glfwKeyMap_["C"] = GLFW_KEY_C;
    glfwKeyMap_["V"] = GLFW_KEY_V;
    glfwKeyMap_["B"] = GLFW_KEY_B;
    glfwKeyMap_["N"] = GLFW_KEY_N;
    glfwKeyMap_["M"] = GLFW_KEY_M;
    glfwKeyMap_["0"] = GLFW_KEY_0;
    glfwKeyMap_["1"] = GLFW_KEY_1;
    glfwKeyMap_["2"] = GLFW_KEY_2;
    glfwKeyMap_["3"] = GLFW_KEY_3;
    glfwKeyMap_["4"] = GLFW_KEY_4;
    glfwKeyMap_["5"] = GLFW_KEY_5;
    glfwKeyMap_["6"] = GLFW_KEY_6;
    glfwKeyMap_["7"] = GLFW_KEY_7;
    glfwKeyMap_["8"] = GLFW_KEY_8;
    glfwKeyMap_["9"] = GLFW_KEY_9;
    glfwKeyMap_["SPACE"] = GLFW_KEY_SPACE;
    glfwKeyMap_["APOSTROPHE"] = GLFW_KEY_APOSTROPHE;
    glfwKeyMap_["COMMA"] = GLFW_KEY_COMMA;
    glfwKeyMap_["MINUS"] = GLFW_KEY_MINUS;
    glfwKeyMap_["PERIOD"] = GLFW_KEY_PERIOD;
    glfwKeyMap_["SLASH"] = GLFW_KEY_SLASH;
    glfwKeyMap_["SEMICOLON"] = GLFW_KEY_SEMICOLON;
    glfwKeyMap_["EQUAL"] = GLFW_KEY_EQUAL;
    glfwKeyMap_["LEFT_BRACKET"] = GLFW_KEY_LEFT_BRACKET;
    glfwKeyMap_["BACKSLASH"] = GLFW_KEY_BACKSLASH;
    glfwKeyMap_["RIGHT_BRACKET"] = GLFW_KEY_RIGHT_BRACKET;
    glfwKeyMap_["GRAVE_ACCENT"] = GLFW_KEY_GRAVE_ACCENT;
    glfwKeyMap_["WORLD_1"] = GLFW_KEY_WORLD_1;
    glfwKeyMap_["WORLD_2"] = GLFW_KEY_WORLD_2;
    glfwKeyMap_["ESCAPE"] = GLFW_KEY_ESCAPE;
    glfwKeyMap_["ENTER"] = GLFW_KEY_ENTER;
    glfwKeyMap_["TAB"] = GLFW_KEY_TAB;
    glfwKeyMap_["BACKSPACE"] = GLFW_KEY_BACKSPACE;
    glfwKeyMap_["INSERT"] = GLFW_KEY_INSERT;
    glfwKeyMap_["DELETE"] = GLFW_KEY_DELETE;
    glfwKeyMap_["RIGHT"] = GLFW_KEY_RIGHT;
    glfwKeyMap_["LEFT"] = GLFW_KEY_LEFT;
    glfwKeyMap_["DOWN"] = GLFW_KEY_DOWN;
    glfwKeyMap_["UP"] = GLFW_KEY_UP;
    glfwKeyMap_["PAGE_UP"] = GLFW_KEY_PAGE_UP;
    glfwKeyMap_["PAGE_DOWN"] = GLFW_KEY_PAGE_DOWN;
    glfwKeyMap_["HOME"] = GLFW_KEY_HOME;
    glfwKeyMap_["END"] = GLFW_KEY_END;
    glfwKeyMap_["CAPS_LOCK"] = GLFW_KEY_CAPS_LOCK;
    glfwKeyMap_["SCROLL_LOCK"] = GLFW_KEY_SCROLL_LOCK;
    glfwKeyMap_["NUM_LOCK"] = GLFW_KEY_NUM_LOCK;
    glfwKeyMap_["PRINT_SCREEN"] = GLFW_KEY_PRINT_SCREEN;
    glfwKeyMap_["PAUSE"] = GLFW_KEY_PAUSE;
    glfwKeyMap_["F1"] = GLFW_KEY_F1;
    glfwKeyMap_["F2"] = GLFW_KEY_F2;
    glfwKeyMap_["F3"] = GLFW_KEY_F3;
    glfwKeyMap_["F4"] = GLFW_KEY_F4;
    glfwKeyMap_["F5"] = GLFW_KEY_F5;
    glfwKeyMap_["F6"] = GLFW_KEY_F6;
    glfwKeyMap_["F7"] = GLFW_KEY_F7;
    glfwKeyMap_["F8"] = GLFW_KEY_F8;
    glfwKeyMap_["F9"] = GLFW_KEY_F9;
    glfwKeyMap_["F10"] = GLFW_KEY_F10;
    glfwKeyMap_["F11"] = GLFW_KEY_F11;
    glfwKeyMap_["F12"] = GLFW_KEY_F12;
    glfwKeyMap_["F13"] = GLFW_KEY_F13;
    glfwKeyMap_["F14"] = GLFW_KEY_F14;
    glfwKeyMap_["F15"] = GLFW_KEY_F15;
    glfwKeyMap_["F16"] = GLFW_KEY_F16;
    glfwKeyMap_["F17"] = GLFW_KEY_F17;
    glfwKeyMap_["F18"] = GLFW_KEY_F18;
    glfwKeyMap_["F19"] = GLFW_KEY_F19;
    glfwKeyMap_["F20"] = GLFW_KEY_F20;
    glfwKeyMap_["F21"] = GLFW_KEY_F21;
    glfwKeyMap_["F22"] = GLFW_KEY_F22;
    glfwKeyMap_["F23"] = GLFW_KEY_F23;
    glfwKeyMap_["F24"] = GLFW_KEY_F24;
    glfwKeyMap_["F25"] = GLFW_KEY_F25;
    glfwKeyMap_["KP_0"] = GLFW_KEY_KP_0;
    glfwKeyMap_["KP_1"] = GLFW_KEY_KP_1;
    glfwKeyMap_["KP_2"] = GLFW_KEY_KP_2;
    glfwKeyMap_["KP_3"] = GLFW_KEY_KP_3;
    glfwKeyMap_["KP_4"] = GLFW_KEY_KP_4;
    glfwKeyMap_["KP_5"] = GLFW_KEY_KP_5;
    glfwKeyMap_["KP_6"] = GLFW_KEY_KP_6;
    glfwKeyMap_["KP_7"] = GLFW_KEY_KP_7;
    glfwKeyMap_["KP_8"] = GLFW_KEY_KP_8;
    glfwKeyMap_["KP_9"] = GLFW_KEY_KP_9;
    glfwKeyMap_["KP_DECIMAL"] = GLFW_KEY_KP_DECIMAL;
    glfwKeyMap_["KP_DIVIDE"] = GLFW_KEY_KP_DIVIDE;
    glfwKeyMap_["KP_MULTIPLY"] = GLFW_KEY_KP_MULTIPLY;
    glfwKeyMap_["KP_SUBTRACT"] = GLFW_KEY_KP_SUBTRACT;
    glfwKeyMap_["KP_ADD"] = GLFW_KEY_KP_ADD;
    glfwKeyMap_["KP_ENTER"] = GLFW_KEY_KP_ENTER;
    glfwKeyMap_["KP_EQUAL"] = GLFW_KEY_KP_EQUAL;
    glfwKeyMap_["LEFT_SHIFT"] = GLFW_KEY_LEFT_SHIFT;
    glfwKeyMap_["LEFT_CONTROL"] = GLFW_KEY_LEFT_CONTROL;
    glfwKeyMap_["LEFT_ALT"] = GLFW_KEY_LEFT_ALT;
    glfwKeyMap_["LEFT_SUPER"] = GLFW_KEY_LEFT_SUPER;
    glfwKeyMap_["RIGHT_SHIFT"] = GLFW_KEY_RIGHT_SHIFT;
    glfwKeyMap_["RIGHT_CONTROL"] = GLFW_KEY_RIGHT_CONTROL;
    glfwKeyMap_["RIGHT_ALT"] = GLFW_KEY_RIGHT_ALT;
    glfwKeyMap_["RIGHT_SUPER"] = GLFW_KEY_RIGHT_SUPER;

    // Reverse keymap, needed for fast key lookup by GLFW value, needed for key callback
    for (auto mapping : glfwKeyMap_)
        glfwKeyMapReverse_[mapping.second] = mapping.first;
}

void InputSystem::loadActionKeyMapFromJsonFile()
{
    nlohmann::json configJson;
    std::ifstream i("../data/config.json");
    i >> configJson;
    auto& actions = configJson["actions"];

    auto addActionLambda = [this, &actions](std::string action) -> void
    {
        addActionKey(action, actions[action]);
    };

    addActionLambda("MOVE_CAMERA_FORWARD");
    addActionLambda("MOVE_CAMERA_BACKWARD");
    addActionLambda("MOVE_CAMERA_LEFT");
    addActionLambda("MOVE_CAMERA_RIGHT");
    addActionLambda("MOVE_CAMERA_UP");
    addActionLambda("MOVE_CAMERA_DOWN");
    addActionLambda("ROLL_CAMERA_LEFT");
    addActionLambda("ROLL_CAMERA_RIGHT");
    addActionLambda("TOGGLE_FREE_ROAM");
    addActionLambda("TOGGLE_GUI");
}

void InputSystem::addActionKey(std::string action, std::string key)
{
    actionKeyMap_[action] = key;
    actionKeyMapReverse_[key] = action;
}
