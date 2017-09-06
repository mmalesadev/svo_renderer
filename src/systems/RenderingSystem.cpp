#include "RenderingSystem.h"

RenderingSystem::RenderingSystem(GLFWwindow* window, int windowWidth, int windowHeight)
{
	shaderProgram_.loadShaderProgram("main");
	camera_.setPerspectiveMatrix(45.0f, (float)windowWidth / (float)windowHeight, 0.2f, 4000.0f);
	camera_.init(window, glm::vec3(0, 0, -50), 0.0f, 0.0f, 5.0f, 0.01f);
}

void RenderingSystem::update(Scene& scene, float deltaTime)
{
	World& world = scene.getWorld();
	auto& entities = world.getEntities();

	camera_.update(deltaTime);

	shaderProgram_.useProgram();	// TODO IMPORTANT: po kolei renderowane wszystkie obiekty, sortowane po shader programie (jak najmniej przelaczen)
	for(auto& entity : entities)
	{
		auto& graphicsComponent = entity->getGraphicsComponent();
		
		// TODO: W zale¿noœci od komponentu ustawiamy uniformy. Wyrzuciæ funkcjê graphicsComponent->setUniforms();
		glm::mat4 projectionMatrix = camera_.getProjectionMatrix();
		glm::mat4 viewMatrix = camera_.getViewMatrix();
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), entity->getPosition()) *
			glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f)) *
			glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::mat4(1.0f) * glm::scale(glm::mat4(1.0f), glm::vec3(entity->getScale()));
		shaderProgram_.setUniform("MV", viewMatrix * modelMatrix);
		shaderProgram_.setUniform("P", projectionMatrix);
		shaderProgram_.setUniform("scale", entity->getScale());
		graphicsComponent->setUniforms(shaderProgram_);
		graphicsComponent->render();
	}
}