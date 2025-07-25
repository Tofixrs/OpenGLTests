file(GLOB_RECURSE SRCFILES CONFIGURE_DEPENDS "./*.cpp")
add_executable(app ${SRCFILES})

set_target_properties(app PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    COMPILE_WARNING_AS_ERROR ON
)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
target_link_libraries(app glfw)
target_link_libraries(app OpenGL)
target_link_libraries(app glad)
target_link_libraries(app glm)
target_link_libraries(app stb)
target_include_directories(app PUBLIC ./include)

