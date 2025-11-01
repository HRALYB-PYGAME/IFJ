add_library(BE_lib
    be.cpp
    game.h
    game.h game.cpp
    gametypes.h
)

target_include_directories(BE_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
