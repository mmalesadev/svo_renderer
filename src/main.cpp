#include "ProgramContext.h"
#include <spdlog/spdlog.h>

int main(int argc, char * argv[])
{
    auto logger = spdlog::basic_logger_mt("logger", "svo_renderer_log.txt");
    logger->set_pattern("[%H:%M:%S][%l] %v");
    logger->set_level(spdlog::level::debug);
        
    ProgramContext programContext;
    programContext.init();
    programContext.run();
}