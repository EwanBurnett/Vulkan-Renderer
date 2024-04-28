project("01-Hello-Triangle")

set(CMAKE_CXX_STANDARD 17) 

add_executable(${PROJECT_NAME} "01-Hello-Triangle.cpp")
target_link_libraries(${PROJECT_NAME} PUBLIC VKR Shared)
add_compile_definitions("$<$<CONFIG:Debug>:DEBUG>")