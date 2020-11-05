#pragma once
#include "System.h"
#include "ProgramVariables.h"

class GuiRenderingSystem : public System
{
public:
    GuiRenderingSystem(std::vector< std::pair<std::string, std::string> >& actionList);
    virtual ~GuiRenderingSystem();
    virtual void update();

private:
    void render();

    bool show_demo_window = false;
    long long nTotalVoxels_ = 0;

    std::pair<int, int> windowSize_;
    std::vector< std::pair<std::string, std::string> >& actionList_;
};
