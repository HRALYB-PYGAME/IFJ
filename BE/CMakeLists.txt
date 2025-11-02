add_library(BE_lib
    game.cpp
    game.h
    # other BE sources
)

# If QWidget is used only in .cpp files:
target_link_libraries(BE_lib PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# If QWidget appears in BE public headers (included by other targets):
# target_link_libraries(BE_lib PUBLIC Qt${QT_VERSION_MAJOR}::Widgets)
