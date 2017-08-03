#include "SceneManager.h"
#include <sqlite3.h>

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
	}
}

void SceneManager::loadSceneFromSqliteDb(std::string sceneName)
{
	sqlite3* db;
	int returnCode = sqlite3_open("data/database.db", &db);
	if (returnCode)
	{
		spdlog::get("console")->critical("Can't open database: {0}", sqlite3_errmsg(db));
		sqlite3_close(db);
		return;
	}

	scenes_[sceneName] = std::make_unique<Scene>();

	auto& loadSceneEntityFromDbCb = [](void *world, int count, char **data, char **columns)
	{
		SPDLOG_DEBUG(spdlog::get("console"), "Loading entity from DB.");
		for (int i = 0; i < count; ++i) SPDLOG_DEBUG(spdlog::get("console"), "{0} = {1}", columns[i], data[i] ? data[i] : nullptr);
		static_cast<World*>(world)->getEntities().push_back(std::make_unique<Entity>(data));
		return 0;
	};

	char* sqlQueryErrorMsg = 0;
	std::string query = "SELECT se.id, e.name, e.graphicsComponent, se.scale, se.posX, se.posY, se.posZ FROM SceneEntities" +
		std::string(" AS se JOIN Scenes AS s ON s.id = se.sceneId JOIN Entities AS e ON e.id = se.entityId WHERE s.name = '") + sceneName + "'";
	returnCode = sqlite3_exec(db,
		query.c_str(),
		loadSceneEntityFromDbCb,
		&scenes_[sceneName]->getWorld(),
		&sqlQueryErrorMsg
		);

	if (returnCode != SQLITE_OK)
	{
		spdlog::get("console")->critical("SQL error: {0}", sqlQueryErrorMsg);
		sqlite3_free(sqlQueryErrorMsg);
	}
	sqlite3_close(db);
	return;
}



