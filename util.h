#pragma once

#include <vector>
#include <string_view>
#include <cstdio>

#include <GLFW/glfw3.h>

void glfw_error_callback(int error, const char* description) {
  fprintf(stderr, "GLFW ERROR! %s\n", description);
}

void glfw_key_callback(GLFWwindow* window_ptr,
                       int key,
                       int scancode,
                       int action,
                       int mods) {
  
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window_ptr, GLFW_TRUE);
  }
}

void glfw_frambuffer_resize_callback(GLFWwindow* window_ptr, int width, int height) {
  glViewport(0, 0, width, height);
}

std::vector<std::string> get_image_paths(const std::string& dataset_path) {
  const std::string rgb_file_path = dataset_path + "rgb.txt";

  FILE* fp = fopen(rgb_file_path.data(), "r");

  if (fp == nullptr) {
    fprintf(stderr, "ERROR! Unable to load rgb file: %s\n", rgb_file_path.data());
  }
  
  char* line_ptr = nullptr;
  size_t n = 0;
  ssize_t read = 0;

  getline(&line_ptr, &n, fp);
  getline(&line_ptr, &n, fp);
  getline(&line_ptr, &n, fp);

  std::vector<std::string> image_paths;

  while ((read = getline(&line_ptr, &n, fp)) != -1) {
    std::string_view line_sw(line_ptr);
    const auto space_position = line_sw.find(' ');

    /* std::string_view timestamp_sw = line_sw.substr(0, space_position); */
    /* std::string timestamp_str = std::string(timestamp_sw); */
    
    std::string_view path_sw = line_sw.substr(space_position + 1, line_sw.size() - space_position - 2);
    
    image_paths.emplace_back(path_sw);
  }

  return image_paths;
}

