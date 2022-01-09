cmake_minimum_required(VERSION 3.16)

project("mineclone")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_CLANG_TIDY clang-tidy)
set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE include-what-you-use)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-g -Wall -Wextra -pedantic -std=c++20 -Werror)

  # Allow for shorter path in __FILE__ macro
  add_compile_options("-ffile-prefix-map=${CMAKE_SOURCE_DIR}=.")
endif()


if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(
    DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.16.1/conan.cmake"
    "${CMAKE_BINARY_DIR}/conan.cmake"
    EXPECTED_HASH SHA256=396e16d0f5eabdc6a14afddbcfff62a54a7ee75c6da23f32f7a31bc85db23484
    TLS_VERIFY ON
  )
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_configure(
  REQUIRES glfw/3.3.6 glad/0.1.34 catch2/2.13.7
  GENERATORS cmake_find_package
  OPTIONS
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

include_directories(${GLFW3_INCLUDE_DIRS})
include_directories(vendor/stb)

add_executable(
  mineclone
  vendor/stb/stb_image.cpp
  src/main.cpp
  src/engine/mineclone.cpp
  src/engine/utils/mat.cpp
  src/engine/utils/logging.cpp
  src/engine/system/renderer.cpp
  src/engine/system/world.cpp
  src/engine/component/renderer_context.cpp
  src/engine/component/shader.cpp
)

target_link_libraries(mineclone PRIVATE glfw::glfw)
target_link_libraries(mineclone PRIVATE glad::glad)

enable_testing()
add_subdirectory(test)
