#include <cstdlib>
#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "gl.h"
#include "util.h"

// TODO(Matias):
// - Display image feed with OpenGL
// - Draw some points on the images


struct Image {
  int width;
  int height;
  int channels;
  const unsigned char* data_ptr;
};


void convert_image_to_greyscale(Image* grey_image_ptr, const Image* rgb_image_ptr) {
  grey_image_ptr->width = rgb_image_ptr->width;
  grey_image_ptr->height = rgb_image_ptr->height;
  grey_image_ptr->channels = 1;
  const size_t gray_buffer_size = sizeof(unsigned char) * grey_image_ptr->width * grey_image_ptr->height;
  unsigned char* gray_buffer_ptr = (unsigned char*)std::malloc(gray_buffer_size);
  
  for (int pixel_index = 0; pixel_index < rgb_image_ptr->width*rgb_image_ptr->height; ++pixel_index) {
    const unsigned char r = rgb_image_ptr->data_ptr[3*pixel_index + 0];
    const unsigned char g = rgb_image_ptr->data_ptr[3*pixel_index + 1];
    const unsigned char b = rgb_image_ptr->data_ptr[3*pixel_index + 2];

    gray_buffer_ptr[pixel_index] = std::min((0.299f*r + 0.587f*g + 0.114f*b), 255.0f);
  }
  
  grey_image_ptr->data_ptr = gray_buffer_ptr;
}


const unsigned char* get_pixel_ptr(int u, int v, const Image* image_ptr) {
  return image_ptr->data_ptr + v * image_ptr->width + u;
}

// TODO(Matias): Reorder these for better cache performance
const int fast_pixel_offsets[] = {
//u   v
  0, -3, // 1
  1, -3, // 2
  2, -2, // 3
  3, -1, // 4
  3,  0, // 5
  3,  1, // 6
  2,  2, // 7
  1,  3, // 8
  0,  3, // 9
 -1,  3, // 10
 -2,  2, // 11
 -3,  1, // 12
 -3,  0, // 13
 -3, -1, // 14
 -2, -2, // 15
 -1, -3, // 16
};
constexpr int number_of_fast_points = 16;

void detect_fast_points(const Image* grey_image_ptr) {

  const auto width = grey_image_ptr->width;
  const auto height = grey_image_ptr->height;

  constexpr int fast_half_size = 3;
  
  for (int v = fast_half_size; v < height - fast_half_size; ++v) {
    for (int u = fast_half_size; u < width - fast_half_size; ++u) {

      for (int fast_pixel_index = 0; fast_pixel_index < number_of_fast_points; ++fast_pixel_index) {
        const int u_offset = fast_pixel_offsets[2*fast_pixel_index + 0];
        const int v_offset = fast_pixel_offsets[2*fast_pixel_index + 1];

        const unsigned char* pixel_ptr = get_pixel_ptr(u + u_offset, v + v_offset, grey_image_ptr);

        unsigned char* pixel_write_ptr = const_cast<unsigned char*>(pixel_ptr);

        *pixel_write_ptr = 0;
      }

      return;
    }
  }
}

int main() {
  std::string dataset_path("dataset/rgbd_dataset_freiburg3_long_office_household/");

  std::vector<std::string> image_paths =  get_image_paths(dataset_path);
  
  std::string image_path = dataset_path + image_paths.at(0);
  
  Image rgb_image = {};
  rgb_image.data_ptr = stbi_load(image_path.data(), &rgb_image.width, &rgb_image.height, &rgb_image.channels, 3);
  
  std::cout << "Image: " << rgb_image.width << "x" << rgb_image.height << ":" << rgb_image.channels << '\n';

  Image grey_image = {};
  
  convert_image_to_greyscale(&grey_image, &rgb_image);
  
  detect_fast_points(&grey_image);

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
    0.9f,  0.9f, 0.0f, 1.0f, 0.0f,
    0.9f, -0.9f, 0.0f, 1.0f, 1.0f,
    -0.9f, -0.9f, 0.0f, 0.0f, 1.0f,
    -0.9f, 0.9f, 0.0f, 0.0f, 0.0f
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
      FragColor = vec4(texture(image_texture, uv).rrr, 1.0);
    }
  
  )GLSL";
  
  const unsigned int image_shader_program = compile_shader_program(image_vs, image_fs);

  unsigned int image_texture = 0;
  glGenTextures(1, &image_texture);
  glBindTexture(GL_TEXTURE_2D, image_texture);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, rgb_image.width, rgb_image.height, 0, GL_RED, GL_UNSIGNED_BYTE, grey_image.data_ptr);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  int uniform_loc = glGetUniformLocation(image_shader_program, "image_texture");

  glUseProgram(image_shader_program);
  glUniform1i(uniform_loc, 0);

  while (!glfwWindowShouldClose(window_ptr)) {

    glClearColor(0.2f, 0.3, 0.4, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glfwSwapBuffers(window_ptr);
    glfwPollEvents();
  }
}
