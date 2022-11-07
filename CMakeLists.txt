cmake_minimum_required (VERSION 3.8)

project ("Omi")

find_package(OpenGL REQUIRED)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

file ( GLOB_RECURSE GLAD_SOURCES "glad_core/*")
add_library ( GLAD ${GLAD_SOURCES} )
target_include_directories ( GLAD PUBLIC ${CMAKE_CURRENT_LIST_DIR}/glad_core/include )

file ( GLOB LIBJSON_SOURCES "libjson/*.c" "libjson/*.h" )
set ( LIBJSON_SOURCES "libjson/json.c" "libjson/json.h")

file ( GLOB OMI_SOURCES "*.cpp" "*.hpp" )
add_executable (Omi ${OMI_SOURCES} ${LIBJSON_SOURCES} )
target_include_directories(Omi PRIVATE ${CMAKE_CURRENT_LIST_DIR}/glm )
target_include_directories(Omi PRIVATE ${CMAKE_CURRENT_LIST_DIR}/glfw3/include )
target_link_libraries(Omi glfw )
target_link_libraries(Omi GLAD )
target_link_libraries(Omi OpenGL::GL)