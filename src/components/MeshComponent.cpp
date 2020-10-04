#include "MeshComponent.h"
#include "SceneManager.h"

MeshComponent::MeshComponent(std::string name)
{
    graphicsComponentType_ = GraphicsComponentType::GRAPHICS_COMPONENT_MESH;

    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);

    //boundingSphereRadius_ = glm::length(midPoint - glm::vec3(maxX, maxY, maxZ));;
}