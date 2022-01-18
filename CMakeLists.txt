cmake_minimum_required(VERSION 3.16)

project("mineclone")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-ggdb -Wall -Wextra -pedantic -std=c++20 -Werror)

  # Allow for shorter path in __FILE__ macro
  add_compile_options("-ffile-prefix-map=${CMAKE_SOURCE_DIR}=.")

  set(CMAKE_CXX_CLANG_TIDY clang-tidy)
  set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE include-what-you-use)
endif()


if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(
    DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.17.0/conan.cmake"
    "${CMAKE_BINARY_DIR}/conan.cmake"
    EXPECTED_HASH SHA256=3bef79da16c2e031dc429e1dac87a08b9226418b300ce004cc125a82687baeef
    TLS_VERIFY ON
  )
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_configure(
  REQUIRES glfw/3.3.6 glad/0.1.34 catch2/2.13.7 imgui/1.86
  GENERATORS cmake_find_package
  OPTIONS
  glad:gl_version=4.6
  glad:gl_profile=core
)

conan_cmake_autodetect(settings)

conan_cmake_install(
  PATH_OR_REFERENCE .
  BUILD missing
  REMOTE conancenter
  SETTINGS ${settings}
)

find_package(glfw3 REQUIRED)
find_package(glad REQUIRED)

add_subdirectory(vendor/imgui)
add_subdirectory(vendor/stb)

add_library(
  mineclone_engine STATIC
  src/engine/mineclone.cpp
  src/engine/utils/mat.cpp
  src/engine/utils/logging.cpp
  src/engine/system/renderer.cpp
  src/engine/system/world.cpp
  src/engine/component/renderer_context.cpp
  src/engine/component/shader.cpp
)

target_include_directories(mineclone_engine PUBLIC src/)

target_link_libraries(mineclone_engine PUBLIC glfw::glfw glad::glad imgui_glfw_opengl stb)

add_executable(
  mineclone
  src/main.cpp
 )

 target_link_libraries(mineclone PRIVATE mineclone_engine)

enable_testing()
add_subdirectory(test)
