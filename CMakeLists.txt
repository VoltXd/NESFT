cmake_minimum_required(VERSION 3.14) 

# *************** No test *************** #
project(simple-nes-emulator LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source & Header location
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
file(GLOB_RECURSE HEADER_FILES include/*.hpp)

file(GLOB_RECURSE GLAD_SOURCE_FILES libraries/glad/src/*.c)

# Make the executable with the headers & sources (NO TESTS)
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${GLAD_SOURCE_FILES})

# Set the directory where CMakeLists.txt is to be the working directory
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

# Target the include folder (used to make headers visible in visual studio)
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE libraries/glad/include)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(libraries/glfw)

target_link_libraries(${PROJECT_NAME} glfw)
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} OpenGL::GL)

# Verbose warnings & warnings are errors
if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()



# *************** Google Test *************** #
project(simple-nes-emulator-TEST LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE TEST_FILES test/*)

# Get google test from github
include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)  
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${TEST_FILES})
target_link_libraries(${PROJECT_NAME} gtest_main)
add_test(NAME example_test COMMAND ${PROJECT_NAME})

# Set the directory where CMakeLists.txt is to be the working directory
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

# Target the include folder (used to make headers visible in visual studio)
target_include_directories(${PROJECT_NAME} PRIVATE include)

#define TEST, so that the program starts with Google Test's main
target_compile_definitions(${PROJECT_NAME} PUBLIC TEST_6502)

# Verbose warnings & warnings are errors
if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()