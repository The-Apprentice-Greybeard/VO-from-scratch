#include <cstdio>
#include <iostream>
#include <vector>

#include <string_view>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// TODO(Matias):
// Add libraries:
//  - stb_image

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


struct Image {
  int width;
  int height;
  int channels;
  unsigned char* data_ptr;
};

int main() {
  std::string dataset_path("dataset/rgbd_dataset_freiburg3_long_office_household/");

  std::vector<std::string> image_paths =  get_image_paths(dataset_path);
  
  std::string image_path = dataset_path + image_paths.at(0);
  
  Image image = {};
  image.data_ptr = stbi_load(image_path.data(), &image.width, &image.height, &image.channels, 0);
  
  std::cout << "Image: " << image.width << "x" << image.height << ":" << image.channels << '\n';

  glfwSetErrorCallback(glfw_error_callback);

  if (!glfwInit()) {
    fprintf(stderr, "ERROR! Unable to init GLFW\n");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  GLFWwindow* window_ptr = glfwCreateWindow(640, 480, "VO From Scratch", nullptr, nullptr);

  if (window_ptr == nullptr) {
    fprintf(stderr, "ERROR! Unable to init GLFW\n");
  }
  
  glfwMakeContextCurrent(window_ptr);
  gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

  glfwSetKeyCallback(window_ptr, glfw_key_callback);

  glfwSwapInterval(1);

  float verticies[] = {
    // x   y    z     u   v
    0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
    0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
  };

  unsigned int indicies[] = {
    0, 1, 3, // first triangle
    1, 2, 3 // second triangle
  };

  unsigned int vbo;
  unsigned int vao;
  unsigned int ebo;

  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  glGenVertexArrays(1, &vao);
  
  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, ebo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(0);


  while (!glfwWindowShouldClose(window_ptr)) {

    glClearColor(0.2f, 0.3, 0.4, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glfwSwapBuffers(window_ptr);
    glfwPollEvents();
  }
}
