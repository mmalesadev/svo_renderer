#include "InputSystem.h"
#include "ProgramVariables.h"
#include <functional>
#include <sqlite3.h>

InputSystem::InputSystem()
{
    // Possible keys used in program are defined here:
    glfwKeyMap_["W"] = GLFW_KEY_W;
    glfwKeyMap_["S"] = GLFW_KEY_S;
    glfwKeyMap_["A"] = GLFW_KEY_A;
    glfwKeyMap_["D"] = GLFW_KEY_D;
    glfwKeyMap_["Q"] = GLFW_KEY_Q;
    glfwKeyMap_["E"] = GLFW_KEY_E;
    glfwKeyMap_["LCTRL"] = GLFW_KEY_LEFT_CONTROL;
    glfwKeyMap_["SPACE"] = GLFW_KEY_SPACE;

    // Possible actions are defined here:
    actionMap_["MOVE_CAMERA_FORWARD"] = std::bind(&InputSystem::moveActiveCamera, this, MovementType::MOVE_FORWARD);
    actionMap_["MOVE_CAMERA_BACKWARD"] = std::bind(&InputSystem::moveActiveCamera, this, MovementType::MOVE_BACKWARD);;
    actionMap_["MOVE_CAMERA_LEFT"] = std::bind(&InputSystem::moveActiveCamera, this, MovementType::MOVE_LEFT);
    actionMap_["MOVE_CAMERA_RIGHT"] = std::bind(&InputSystem::moveActiveCamera, this, MovementType::MOVE_RIGHT);
    actionMap_["ROLL_CAMERA_LEFT"] = std::bind(&InputSystem::moveActiveCamera, this, MovementType::ROLL_LEFT);
    actionMap_["ROLL_CAMERA_RIGHT"] = std::bind(&InputSystem::moveActiveCamera, this, MovementType::ROLL_RIGHT);
    actionMap_["MOVE_CAMERA_UP"] = std::bind(&InputSystem::moveActiveCamera, this, MovementType::MOVE_UP);
    actionMap_["MOVE_CAMERA_DOWN"] = std::bind(&InputSystem::moveActiveCamera, this, MovementType::MOVE_DOWN);

    loadActionKeyMapFromSqliteDb();
}

void InputSystem::update(float deltaTime)
{
    // Checking all possible keys pressed and executing assigned actions
    for (auto it = keyActionMap_.begin(); it != keyActionMap_.end(); ++it)
    {
        if (glfwGetKey(ProgramVariables::getWindow(), glfwKeyMap_[it->first]) == GLFW_PRESS)
        {
            auto& actionFunc = actionMap_[keyActionMap_[it->first]];
            if(actionFunc) actionFunc();
        }
    }

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
            glm::quat deltaRoll = glm::angleAxis(-0.02f * ProgramVariables::getDeltaTime(), cameraComponent->getDirection());
            transformComponent->setQuaternion(glm::normalize(deltaRoll * transformComponent->getQuaternion()));
            break;
        }
        case MovementType::ROLL_RIGHT:
        {
            glm::quat deltaRoll = glm::angleAxis(0.02f * ProgramVariables::getDeltaTime(), cameraComponent->getDirection());
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

void InputSystem::loadActionKeyMapFromSqliteDb()
{
    sqlite3* db;
    int returnCode = sqlite3_open("data/database.db", &db);
    if (returnCode)
    {
        spdlog::get("console")->critical("Can't open database: {0}", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    auto& loadActionKeysFromDbCb = [](void *inputSystem, int count, char **data, char **columns)
    {
        SPDLOG_DEBUG(spdlog::get("console"), "Loading action keys from DB.");
        for (int i = 0; i < count; ++i) SPDLOG_DEBUG(spdlog::get("console"), "{0} = {1}", columns[i], data[i] ? data[i] : nullptr);
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
        spdlog::get("console")->critical("SQL error: {0}", sqlQueryErrorMsg);
        sqlite3_free(sqlQueryErrorMsg);
    }
    sqlite3_close(db);

    // TODO: Zmieniaæ dziedzinê mappingów (czyli actionMap) 
    // w zale¿noœci od sceny w jakiej aktualnie jesteœmy
    // (albo te¿ innych zmiennych?) Daæ mapê z map¹ <sceneName, mapa_dziedziny>?
    // Iterowac tylko po mapie danej sceny

    // TODO: wyrzuciæ i wczytaæ z bazy jak bd dzia³aæ
    //actionMap_["moveCameraForward"] = moveCameraForward;;
}

void InputSystem::addActionKey(std::string action, std::string key)
{
    keyActionMap_[key] = action;
}
