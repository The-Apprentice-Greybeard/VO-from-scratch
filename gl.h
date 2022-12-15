#pragma once
#include <cstdio>

#include <glad/glad.h>

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
