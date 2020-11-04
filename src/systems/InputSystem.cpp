#include "InputSystem.h"
#include "ProgramVariables.h"
#include <functional>
#include <sqlite3.h>

InputSystem::InputSystem() : inputMode_(InputMode::GUI_MODE)
{
    prepareGlfwKeyMaps();

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
    
    loadActionKeyMapFromSqliteDb();

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
    auto activeCamera = SceneManager::getInstance()->getActiveScene()->getActiveCamera();
    if (activeCamera)
    {
        auto& transformComponent = activeCamera->getTransformComponent();
        auto& cameraComponent = activeCamera->getCameraComponent();
        glm::quat deltaPitch = glm::angleAxis(0.002f * float(height / 2 - yPos), cameraComponent->getRight());
        glm::quat deltaYaw = glm::angleAxis(0.002f * float(width / 2 - xPos), cameraComponent->getUp());
        transformComponent->setQuaternion(glm::normalize(deltaPitch * deltaYaw * transformComponent->getQuaternion()));
        cameraComponent->update(*transformComponent);
    }
}

void InputSystem::moveActiveCamera(MovementType movementType)
{
    auto activeScene = SceneManager::getInstance()->getActiveScene();
    auto activeCamera = activeScene->getActiveCamera();
    if (activeCamera)
    {
        auto& transformComponent = activeCamera->getTransformComponent();
        auto& cameraComponent = activeCamera->getCameraComponent();

        switch (movementType)
        {
        case MovementType::MOVE_FORWARD:
            transformComponent->setPosition(transformComponent->getPosition() + cameraComponent->getDirection() * ProgramVariables::getDeltaTime() * cameraComponent->getSpeed());
            break;
        case MovementType::MOVE_BACKWARD:
            transformComponent->setPosition(transformComponent->getPosition() - cameraComponent->getDirection() * ProgramVariables::getDeltaTime() * cameraComponent->getSpeed());
            break;
        case MovementType::MOVE_LEFT:
            transformComponent->setPosition(transformComponent->getPosition() - cameraComponent->getRight() * ProgramVariables::getDeltaTime() * cameraComponent->getSpeed());
            break;
        case MovementType::MOVE_RIGHT:
            transformComponent->setPosition(transformComponent->getPosition() + cameraComponent->getRight() * ProgramVariables::getDeltaTime() * cameraComponent->getSpeed());
            break;
        case MovementType::ROLL_LEFT:
        {
            glm::quat deltaRoll = glm::angleAxis(-1.0f * ProgramVariables::getDeltaTime(), cameraComponent->getDirection());
            transformComponent->setQuaternion(glm::normalize(deltaRoll * transformComponent->getQuaternion()));
            break;
        }
        case MovementType::ROLL_RIGHT:
        {
            glm::quat deltaRoll = glm::angleAxis(1.0f * ProgramVariables::getDeltaTime(), cameraComponent->getDirection());
            transformComponent->setQuaternion(glm::normalize(deltaRoll * transformComponent->getQuaternion()));
            break;
        }
        case MovementType::MOVE_UP:
            transformComponent->setPosition(transformComponent->getPosition() + ProgramVariables::getDeltaTime() * cameraComponent->getSpeed() * cameraComponent->getUp());
            break;
        case MovementType::MOVE_DOWN:
            transformComponent->setPosition(transformComponent->getPosition() - ProgramVariables::getDeltaTime() * cameraComponent->getSpeed() * cameraComponent->getUp());
            break;
        default:
            break;
        }

        cameraComponent->update(*transformComponent);
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

void InputSystem::prepareGlfwKeyMaps()
{
    // Possible keys used in program are defined here:
    glfwKeyMap_["W"] = GLFW_KEY_W;
    glfwKeyMap_["S"] = GLFW_KEY_S;
    glfwKeyMap_["A"] = GLFW_KEY_A;
    glfwKeyMap_["D"] = GLFW_KEY_D;
    glfwKeyMap_["Q"] = GLFW_KEY_Q;
    glfwKeyMap_["E"] = GLFW_KEY_E;
    glfwKeyMap_["R"] = GLFW_KEY_R;
    glfwKeyMap_["LCTRL"] = GLFW_KEY_LEFT_CONTROL;
    glfwKeyMap_["SPACE"] = GLFW_KEY_SPACE;

    // Reverse keymap, needed for fast key lookup by GLFW value, needed for key callback
    for (auto mapping : glfwKeyMap_)
        glfwKeyMapReverse_[mapping.second] = mapping.first;
}

void InputSystem::loadActionKeyMapFromSqliteDb()
{
    sqlite3* db;
    int returnCode = sqlite3_open("../data/database.db", &db);
    if (returnCode)
    {
        spdlog::get("logger")->critical("Can't open database: {0}", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    auto& loadActionKeysFromDbCb = [](void *inputSystem, int count, char **data, char **columns)
    {
        SPDLOG_DEBUG(spdlog::get("logger"), "Loading action keys from DB.");
        for (int i = 0; i < count; ++i) SPDLOG_DEBUG(spdlog::get("logger"), "{0} = {1}", columns[i], data[i] ? data[i] : nullptr);
        static_cast<InputSystem*>(inputSystem)->addActionKey(std::string(data[0]), std::string(data[1]));
        return 0;
    };

    char* sqlQueryErrorMsg = 0;
    std::string query = "SELECT ak.action, ak.key FROM ActionKeys AS ak";
    returnCode = sqlite3_exec(db,
        query.c_str(),
        loadActionKeysFromDbCb,
        this,
        &sqlQueryErrorMsg
    );

    if (returnCode != SQLITE_OK)
    {
        spdlog::get("logger")->critical("SQL error: {0}", sqlQueryErrorMsg);
        sqlite3_free(sqlQueryErrorMsg);
    }
    sqlite3_close(db);
}

void InputSystem::addActionKey(std::string action, std::string key)
{
    actionKeyMap_[action] = key;
    actionKeyMapReverse_[key] = action;
}
