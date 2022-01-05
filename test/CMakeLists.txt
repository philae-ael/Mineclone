include(CTest)

enable_testing()

find_package(Catch2 2 REQUIRED)
include(Catch)


add_executable(mineclone_tests
  main.cpp
  math.cpp
  math_opengl.cpp
  range_it.cpp
  )

target_compile_options(mineclone_tests PRIVATE -Og)

target_include_directories(mineclone_tests PRIVATE ../src/)
target_link_libraries(mineclone_tests PRIVATE Catch2::Catch2)

catch_discover_tests(mineclone_tests)
