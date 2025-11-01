add_library(BE
    be.cpp
    game.h
    game.h game.cpp
    gametypes.h
)

target_include_directories(BE PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
