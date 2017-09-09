#include "ProgramContext.h"
#include <spdlog/spdlog.h>

int main(int argc, char * argv[])
{
    auto console = spdlog::stdout_color_mt("console");
    console->set_pattern("[%H:%M:%S][%l] %v");
    console->set_level(spdlog::level::debug);
        
    ProgramContext programContext;
    programContext.init();
    programContext.run();
}