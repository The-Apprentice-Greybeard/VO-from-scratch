#include <cstdio>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// TODO(Matias):
// Add libraries:
//  - glfw
//  - glad
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

int main() {

  if (!glfwInit()) {
    fprintf(stderr, "ERROR! Unable to init GLFW\n");
  }

  glfwSetErrorCallback(glfw_error_callback);

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

  while (!glfwWindowShouldClose(window_ptr)) {

    glClearColor(0.2f, 0.3, 0.4, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glfwSwapBuffers(window_ptr);
    glfwPollEvents();
  }
}
