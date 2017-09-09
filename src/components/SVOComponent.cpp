#include "SVOComponent.h"

SVOComponent::SVOComponent(std::string name) : octree(std::make_unique<Octree>(name))
{
    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
    
    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    auto octreeData = octree->getData();
    glBufferData(GL_ARRAY_BUFFER, octree->getData().size() * sizeof(Octree::Data), &octreeData[0], GL_STATIC_DRAW);
    //float vertices[] = {
    //    -0.5f, -0.5f,
    //    0.5f, -0.5f, // Vertex 2 (X, Y)
  // Vertex 3 (X, Y)
    //    0.0f,  0.5f
    //};
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Octree::Data), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Octree::Data), (GLvoid*)offsetof(Octree::Data, color));

    //glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void SVOComponent::setUniforms(ShaderProgram& shaderProgram)
{
    shaderProgram.setUniform("gridLength", (float) octree->getHeader().gridLength);
}

void SVOComponent::render()
{
    //printf("Rendering component\n");
    glBindVertexArray(VAO_);
    glDrawArrays(GL_POINTS, 0, octree->getData().size());

    //SPDLOG_DEBUG(spdlog::get("console"), "{0} {1} {2}", octree->getData()[1].color.x, octree->getData()[1].color.y, octree->getData()[1].color.z);
}
