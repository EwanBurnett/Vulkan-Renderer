project("00-DEV")

set(CMAKE_CXX_STANDARD 17) 

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC VKR)
add_compile_definitions("$<$<CONFIG:Debug>:DEBUG>")