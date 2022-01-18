find_package(imgui REQUIRED)

add_library(imgui_glfw_opengl STATIC
  ${imgui_RES_DIRS}/bindings/imgui_impl_glfw.cpp
  ${imgui_RES_DIRS}/bindings/imgui_impl_opengl3.cpp
  )

target_link_libraries(imgui_glfw_opengl PUBLIC imgui::imgui)
target_link_libraries(imgui_glfw_opengl PRIVATE glad::glad)
target_link_libraries(imgui_glfw_opengl PRIVATE glfw::glfw)
target_include_directories(imgui_glfw_opengl INTERFACE ${imgui_RES_DIRS})
