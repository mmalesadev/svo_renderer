#include "RenderingSystem.h"

RenderingSystem::RenderingSystem(GLFWwindow* window, int windowWidth, int windowHeight)
{
	shaderProgram_.loadShaderProgram("main");
	camera_.setPerspectiveMatrix(45.0f, (float)windowWidth / (float)windowHeight, 0.2f, 4000.0f);
	camera_.init(window, glm::vec3(0, 0, -50), 0.0f, 0.0f, 300.0f, 0.01f);
}

void RenderingSystem::update(Scene& scene, float deltaTime)
{
	World& world = scene.getWorld();
	auto& entities = world.getEntities();

	camera_.update(deltaTime);
	//printf("eye: %f", camera_.eye.x);

	shaderProgram_.useProgram();	// TODO IMPORTANT: po kolei renderowane wszystkie obiekty, sortowane po shader programie (jak najmniej przelaczen)
	for(auto& entity : entities)
	{
		auto& graphicsComponent = entity->getGraphicsComponent();

		// TODO: renderujemy u¿ywaj¹c funkcji z GraphicsComponent.
		glm::mat4 projectionMatrix = camera_.getProjectionMatrix();
		glm::mat4 viewMatrix = camera_.getViewMatrix();
		//printf("Entity pos: %f %f %f", entity->getPosition().x, entity->getPosition().y, entity->getPosition().z);
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), entity->getPosition()) *
			glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f)) *
			glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::mat4(1.0f);
		glm::mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;
		shaderProgram_.setUniform("MVP", MVP);

		graphicsComponent->render();
	}
}