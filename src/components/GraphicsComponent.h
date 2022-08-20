#pragma once

#include <array>
#include <map>
#include <any>
#include <vector>
#include <string>
#include "Shader.h"
#include "OctreeFile.h"
#include <spdlog/spdlog.h>

class GraphicsComponent
{
public:
    GraphicsComponent();
    GraphicsComponent(std::string name, glm::vec4 color);

    std::string getName() const { return name_; }
    glm::vec4 getColor() const { return color_; }
    uint8_t getLod() const { return currentSvoLodIdx_ + 1; }
    void setColor(glm::vec4 color) { color_ = color; }
    void setLod(uint8_t lod) { currentSvoLodIdx_ = lod - 1; }
    float getBoundingSphereRadius() { return boundingSphereRadius_; }

    unsigned int getGridLength() const { return octreeFiles_[currentSvoLodIdx_]->getHeader().gridLength; }
    size_t getDataSize() const { return octreeFiles_[currentSvoLodIdx_]->getData().size(); }
    void setVisible(bool visible) { visible_ = visible; }
    bool isVisible() const { return visible_; }

    GLuint getVAO() const { return VAOs_[currentSvoLodIdx_]; }
    uint8_t getLevelOfDetail() const { return currentSvoLodIdx_ + 1; }
    void decreaseLevelOfDetail();
    void increaseLevelOfDetail();

protected:
    enum class GraphicsComponentType { GRAPHICS_COMPONENT_SVO };
    GraphicsComponentType graphicsComponentType_;

    uint8_t maxSvoDepth_;
    uint8_t currentSvoLodIdx_;
    std::vector<std::shared_ptr<OctreeFile>> octreeFiles_;

    std::vector<GLuint> VAOs_;
    std::vector<GLuint> VBOs_;

    float boundingSphereRadius_;

    std::map<std::string, std::any> uniformMap;

    bool visible_;

    std::string name_;
    glm::vec4 color_;
};