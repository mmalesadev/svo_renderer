#pragma once

#include <memory>
#include "GraphicsComponent.h"
#include "ControllerComponent.h"
#include <spdlog/spdlog.h>
#include "SVOComponent.h"

class Entity
{
public:
	Entity(char **data) : id_(std::stoi(data[0])), name_(data[1]), scale_(std::stof(data[3])),
		position_(glm::vec3(std::stof(data[4]), std::stof(data[5]), std::stof(data[6])))
	{
		SPDLOG_DEBUG(spdlog::get("console"), "Constructing entity. id: {0}, name: {1}", id_, data[1]);

		// Deciding on graphics component type by file extension
		// TODO: Przerzuæ do entity creator
		// Dodaj name do Entity i w bazie danych w tabeli SceneEntities
		std::string graphicsComponentType(data[2]);
		if (graphicsComponentType == "svo")
		{
			SPDLOG_DEBUG(spdlog::get("console"), "Constructing SVOComponent.");
			graphicsComponent_ = std::make_unique<SVOComponent>(name_, position_, scale_);
		}
	}

	~Entity()
	{
		SPDLOG_DEBUG(spdlog::get("console"), "Destructing entity.");
	}

	Entity(const Entity&)
	{
		SPDLOG_DEBUG(spdlog::get("console"), "Copy constructing entity.");
	}

	std::string getName() { return name_; }
	float getScale() { return scale_; }
	glm::vec3 getPosition() { return position_; }
	std::unique_ptr<GraphicsComponent>& getGraphicsComponent() { return graphicsComponent_; }
	std::unique_ptr<ControllerComponent>& getControllerComponent() { return controllerComponent_; }

private:
	unsigned int id_;
	std::string name_;
	float scale_;
	glm::vec3 position_;
	std::unique_ptr<GraphicsComponent> graphicsComponent_;
	std::unique_ptr<ControllerComponent> controllerComponent_;
};