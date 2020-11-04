#include "SceneManager.h"
#include <sqlite3.h>
#include "SVOComponent.h"
#include "MeshComponent.h"

SceneManager* SceneManager::instance_ = nullptr;

SceneManager::SceneManager()
{
    instance_ = this;
}

SceneManager::~SceneManager()
{
    instance_ = nullptr;
}

std::unique_ptr<SceneManager> SceneManager::createInstance()
{
    if (!instance_)
    {
        std::unique_ptr<SceneManager> sceneManager(new SceneManager());
        return sceneManager;
    }
    else return nullptr;
}

SceneManager* SceneManager::getInstance()
{
    return instance_;
}

void SceneManager::activateScene(std::string sceneName)
{
    auto scene = scenes_.find(sceneName);
    if (scene != scenes_.end())
    {
        activeScene_ = scene->second.get();
        return;
    }
    else
    {
        loadSceneFromSqliteDb(sceneName);
        activeScene_ = scenes_[sceneName].get();
        activeScene_->activateFirstFoundCamera();
    }
}

Scene* SceneManager::getActiveScene()
{
    return activeScene_;
}

void SceneManager::loadSceneFromSqliteDb(std::string sceneName)
{
    sqlite3* db;
    int returnCode = sqlite3_open("../data/database.db", &db);
    if (returnCode)
    {
        spdlog::get("logger")->critical("Can't open database: {0}", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    scenes_[sceneName] = std::make_unique<Scene>();

    auto& loadSceneEntityFromDbCb = [](void *world, int count, char **data, char **columns)
    {
        SPDLOG_DEBUG(spdlog::get("logger"), "Loading entity from DB.");
        for (int i = 0; i < count; ++i) SPDLOG_DEBUG(spdlog::get("logger"), "{0} = {1}", columns[i], data[i] ? data[i] : nullptr);

        // CONSTRUCTING ENTITY FROM COMPONENTS 
        // TODO: Przerzuæ do entity creator
        // Dodaj name do Entity i w bazie danych w tabeli SceneEntities
        std::unique_ptr<TransformComponent> newTransformComponent;
        std::unique_ptr<GraphicsComponent> newGraphicsComponent;
        std::unique_ptr<CameraComponent> newCameraComponent;

        newTransformComponent = std::make_unique<TransformComponent>(std::stof(data[3]),
            glm::vec3(std::stof(data[4]), std::stof(data[5]), std::stof(data[6])),
            glm::vec3(glm::radians(std::stof(data[7])), glm::radians(std::stof(data[8])), glm::radians(std::stof(data[9])))
            );
        std::string graphicsComponentType(data[2]);
        if (graphicsComponentType == "svo")
        {
            SPDLOG_DEBUG(spdlog::get("logger"), "Constructing SVOComponent. Name: {0}", data[1]);
            newGraphicsComponent = std::make_unique<SVOComponent>(data[1]);
        }
        else if (graphicsComponentType == "mesh")
        {
            SPDLOG_DEBUG(spdlog::get("logger"), "Constructing Mesh. Name: {0}", data[1]);
            newGraphicsComponent = std::make_unique<MeshComponent>(data[1]);
        }
        std::string cameraComponentType(data[10]);
        if (cameraComponentType == "perspective")
        {
            newCameraComponent = std::make_unique<CameraComponent>(std::stof(data[11]));
        }

        static_cast<World*>(world)->getEntities().push_back(std::make_unique<Entity>(
            std::stoi(data[0]),        // id
            std::string(data[1]),                // name
            newTransformComponent,
            newGraphicsComponent,
            newCameraComponent
            ));
        return 0;
    };

    char* sqlQueryErrorMsg = 0;
    std::string query = "SELECT se.id, e.name, e.graphicsComponent, se.scale, se.posX, se.posY, se.posZ, se.rotX, se.rotY, se.rotZ, e.cameraComponent, e.cameraSpeed FROM SceneEntities" +
        std::string(" AS se JOIN Scenes AS s ON s.id = se.sceneId JOIN Entities AS e ON e.id = se.entityId WHERE s.name = '") + sceneName + "'";
    returnCode = sqlite3_exec(db,
        query.c_str(),
        loadSceneEntityFromDbCb,
        &scenes_[sceneName]->getWorld(),
        &sqlQueryErrorMsg
        );

    if (returnCode != SQLITE_OK)
    {
        spdlog::get("logger")->critical("SQL error: {0}", sqlQueryErrorMsg);
        sqlite3_free(sqlQueryErrorMsg);
    }
    sqlite3_close(db);
    return;
}



