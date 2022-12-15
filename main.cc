#include <cstdio>
#include <iostream>
#include <vector>

#include <string_view>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// TODO(Matias):
// - Display image feed with OpenGL
// - Draw some points on the images

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

void glad_gl_post_callback(const char* function_name_ptr, void *funcptr, int len_args, ...) {
    GLenum error_code = glad_glGetError();

    if (error_code != GL_NO_ERROR) {
      const char* error_name = nullptr;
      switch(error_code) {
        case GL_INVALID_ENUM: { {}
          error_name = "GL_INVALID_ENUM";
        } break;
        case GL_INVALID_VALUE: {
          error_name = "GL_INVALID_VALUE";
        } break;
        case GL_INVALID_OPERATION: {
          error_name = "GL_INVALID_OPERATION";
        } break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: {
          error_name = "GL_INVALID_FRAMEBUFFER_OPERATION";
        } break;
        case GL_OUT_OF_MEMORY: {
          error_name = "GL_OUT_OF_MEMORY";
        } break;
    }

        fprintf(stderr, "OpenGL ERROR! %s: %s\n", function_name_ptr, error_name);
    }
}



unsigned int compile_shader_program(
  const char* vertex_shader_source_ptr,
  const char* fragment_shader_source_ptr
) {
  constexpr size_t log_size = 1024;
  char error_log[log_size];
  int error_status;
  
  unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_source_ptr, nullptr);
  glCompileShader(vertex_shader);

  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &error_status);
  if(!error_status) {
    glGetShaderInfoLog(vertex_shader, log_size, nullptr, error_log);
    fprintf(stderr, "GLSL Vertex Shader Error!\n%s", error_log);
  }

  unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_source_ptr, nullptr);
  glCompileShader(fragment_shader);

  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &error_status);
  if(!error_status) {
    glGetShaderInfoLog(fragment_shader, log_size, nullptr, error_log);
    fprintf(stderr, "GLSL Fragment Shader Error!\n%s", error_log);
  }
  
  unsigned int shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);

  glGetProgramiv(shader_program, GL_LINK_STATUS, &error_status);
  if(!error_status) {
    glGetShaderInfoLog(shader_program, log_size, nullptr, error_log);
    fprintf(stderr, "GLSL Shader Program Error!\n%s", error_log);
  }

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  return shader_program;
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
  image.data_ptr = stbi_load(image_path.data(), &image.width, &image.height, &image.channels, 3);
  
  std::cout << "Image: " << image.width << "x" << image.height << ":" << image.channels << '\n';

  glfwSetErrorCallback(glfw_error_callback);

  if (!glfwInit()) {
    fprintf(stderr, "ERROR! Unable to init GLFW\n");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  GLFWwindow* window_ptr = glfwCreateWindow(1280, 1080, "VO From Scratch", nullptr, nullptr);

  if (window_ptr == nullptr) {
    fprintf(stderr, "ERROR! Unable to init GLFW\n");
  }
  
  glfwMakeContextCurrent(window_ptr);
  glad_set_post_callback(glad_gl_post_callback);
  gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
  
  glfwSetFramebufferSizeCallback(window_ptr, glfw_frambuffer_resize_callback);
  glfwSetKeyCallback(window_ptr, glfw_key_callback);

  glfwSwapInterval(1);

  float verticies[] = {
    // x   y    z     u   v
    0.9f,  0.9f, 0.0f, 1.0f, 1.0f,
    0.9f, -0.9f, 0.0f, 1.0f, 0.0f,
    -0.9f, -0.9f, 0.0f, 0.0f, 0.0f,
    -0.9f, 0.9f, 0.0f, 0.0f, 1.0f
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

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  

  const char* image_vs = R"GLSL(
    #version 330 core

    layout (location = 0) in vec3 vs_pos;
    layout (location = 1) in vec2 vs_uv;

    out vec2 uv;

    void main() {
      gl_Position = vec4(vs_pos, 1.0);
      uv = vs_uv;
    }
  )GLSL";

  const char* image_fs = R"GLSL(
    #version 330 core

    out vec4 FragColor;

    in vec2 uv;

    uniform sampler2D image_texture;
    
    void main() {
      FragColor = vec4(texture(image_texture, uv).rgb, 1.0);
    }
  
  )GLSL";
  
  const unsigned int image_shader_program = compile_shader_program(image_vs, image_fs);

  unsigned int image_texture = 0;
  glGenTextures(1, &image_texture);
  glBindTexture(GL_TEXTURE_2D, image_texture);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data_ptr);
  
  int uniform_loc = glGetUniformLocation(image_shader_program, "image_texture");
  printf("uniform_loc %i\n", uniform_loc);

  glUniform1i(uniform_loc, 0);

  while (!glfwWindowShouldClose(window_ptr)) {

    glClearColor(0.2f, 0.3, 0.4, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(image_shader_program);
    glBindVertexArray(vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glfwSwapBuffers(window_ptr);
    glfwPollEvents();
  }
}
