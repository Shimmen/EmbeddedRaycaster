cmake_minimum_required(VERSION 3.5)
project(EmbeddedRaycaster C)

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c99")

file(GLOB SOURCE_FILES src/*.h src/*.c)
add_executable(EmbeddedRaycaster ${SOURCE_FILES})

include(cmake/FindSDL2.cmake)
if (SDL2_FOUND)
    target_include_directories(EmbeddedRaycaster PUBLIC ${SDL2_INCLUDE_DIR})
    target_link_libraries(EmbeddedRaycaster PUBLIC ${SDL2_LIBRARY})
endif ()
