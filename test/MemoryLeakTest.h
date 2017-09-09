#ifdef _WIN32
#include <catch.hpp>
#include <crtdbg.h>
#include "ProgramContext.h"
#include <iostream>

struct CrtMemoryCheck
{
    _CrtMemState startState;
    _CrtMemState endState;
    _CrtMemState stateDiff;

    CrtMemoryCheck()
    {
        _CrtMemCheckpoint(&startState);
    }

    ~CrtMemoryCheck()
    {
        _CrtMemCheckpoint(&endState);
        REQUIRE(0 == _CrtMemDifference(&stateDiff, &startState, &endState));
    }
};

TEST_CASE( "Initializing whole program" )
{
    auto console = spdlog::stdout_color_mt("console");
    console->set_pattern("[%H:%M:%S][%l] %v");
    console->set_level(spdlog::level::debug);

    CrtMemoryCheck crtMemoryCheck;

    ProgramContext programContext;
    programContext.init();
}

#endif
