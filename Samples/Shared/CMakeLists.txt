project("Shared")

add_library(${PROJECT_NAME} STATIC 
    "DemoApp.h"
    "DemoApp.cpp"
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})