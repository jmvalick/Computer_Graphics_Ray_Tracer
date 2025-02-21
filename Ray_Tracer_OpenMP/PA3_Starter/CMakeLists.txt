cmake_minimum_required(VERSION 3.1)
project(CMPSC458_Projects)

set (CMAKE_CXX_STANDARD 14)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(Vendor/glfw)

option(ASSIMP_BUILD_GLTF_IMPORTER OFF)
option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
OPTION( ASSIMP_BUILD_ZLIB ON )
add_subdirectory(Vendor/assimp)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++0x")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()


set(DCMAKE_BUILD_TYPE 
       debug)

set(PROJECTS
    Project_3
)

include_directories(Vendor/assimp/include/
                    Vendor/glad/include/
                    Vendor/glfw/include/
                    Vendor/glm/
                    Vendor/stb/)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()
                    
                    
foreach(PROJECT ${PROJECTS})

    file(GLOB VENDORS_SOURCES Vendor/glad/src/glad.c)
    file(GLOB PROJECT_HEADERS ${PROJECT}/Headers/*.h
                              ${PROJECT}/Headers/*.hpp)
    file(GLOB PROJECT_SOURCES ${PROJECT}/Sources/*.cpp)
    file(GLOB PROJECT_SHADERS ${PROJECT}/Shaders/*.comp
                              ${PROJECT}/Shaders/*.frag
                              ${PROJECT}/Shaders/*.geom
                              ${PROJECT}/Shaders/*.vert)
    file(GLOB PROJECT_CONFIGS CMakeLists.txt
                              Readme.md)
                              
                              
    MESSAGE( STATUS "Headers for " ${PROJECT} " : "  ${PROJECT_HEADERS} )
    source_group("Headers" FILES ${PROJECT_HEADERS})
    source_group("Shaders" FILES ${PROJECT_SHADERS})
    source_group("Sources" FILES ${PROJECT_SOURCES})
    source_group("Vendors" FILES ${VENDORS_SOURCES})

    add_definitions(-DGLFW_INCLUDE_NONE
                    -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")
    add_executable(${PROJECT} ${PROJECT_SOURCES} ${PROJECT_HEADERS}
                                   ${PROJECT_SHADERS} ${PROJECT_CONFIGS}
                                   ${VENDORS_SOURCES})
                                   
    target_include_directories(${PROJECT} PUBLIC 
                               ${PROJECT}/Headers/)
                               
    target_link_libraries(${PROJECT} assimp glfw
                          ${GLFW_LIBRARIES} ${GLAD_LIBRARIES})
    set_target_properties(${PROJECT} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT})
    
endforeach(PROJECT)
