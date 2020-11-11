#pragma once
#include "System.h"
#include "SceneManager.h"
#include <vector>
#include <string>

class GuiSystem : public System
{
public:
    GuiSystem(std::vector< std::pair<std::string, std::string> >& actionList);
    virtual ~GuiSystem();
    virtual void update();

private:
    void render();

    void renderEntityColorPicker(EntityId entity);

    bool show_demo_window = false;

    std::pair<int, int> windowSize_;
    std::vector< std::pair<std::string, std::string> >& actionList_;
};
