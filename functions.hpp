#ifndef CG_LABS_FUNCTIONS_HPP
#define CG_LABS_FUNCTIONS_HPP
#if defined(__APPLE__)
#define LOGURU_WITH_STREAMS 1
  #include "libs/loguru.cpp"
#endif

#if defined(__WIN32__)
#include "libs/easylogging++.h"
INITIALIZE_EASYLOGGINGPP
#define LOG_S LOG
#endif

#include <glad/glad.h>// should be before glfw3 include or compilation will fail
#include <GLFW/glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#if defined(__APPLE__)
#define ASSERT(X) \
	if (!(X)) __builtin_trap()
#endif

#if defined(__WIN32__)
#define ASSERT(X) \
	if (!(X)) __debugbreak()
#endif
/**
 * @brief checks if GL function call failed or succeeded
 **/
#define glCall(x)  \
  glClearErrors(); \
  x;               \
  ASSERT(glLogCall(#x, __FILE__, __LINE__))
#if defined(__WIN32__)
#define  uint unsigned int
#endif
/**
 * @brief checks if program being built on windows
 * @return true if on Windows, false otherwise
 **/
[[maybe_unused]] bool isWindows() {
#if defined(__WIN32__)
  return true;
#endif
  return false;
}
/**
 * @brief checks if program being built on macOS
 * @return true if on macOS, false otherwise
 **/
bool isMac() {
#if defined(__APPLE__)
  return true;
#endif
  return false;
}
/**
 * @brief checks if program being built on Linux
 * @return true if on Linux, false otherwise
 **/
bool isLinux() {
#if defined(__unix__)
  return true;
#endif
  return false;
}
std::string glErrorToString(GLenum error) {
  switch (error) {
    case GL_INVALID_ENUM: return "INVALID ENUM";
    case GL_INVALID_VALUE: return "INVALID VALUE";
    case GL_INVALID_OPERATION: return "INVALID OPERATION";
    case GL_STACK_OVERFLOW: return " STACK OVERFLOW";
    case GL_STACK_UNDERFLOW: return " STACK UNDERFLOW";
    case GL_OUT_OF_MEMORY: return "OUT OF MEMORY";
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "INVALID FRAMEBUFFER OPERATION";
    default: return std::to_string(error);
  }
}

/**
 * @brief checks opengl errors
 * @warning do not call this function directly, use glCall() instead
 **/
bool glLogCall(const char *function = {}, const char *file = {}, int line = -1) {
  if (function == nullptr && file == nullptr && line == -1) {
    LOG_S(ERROR) << "glLogCall() should not be called directly, wrap GL call in glCall()";
    return true;
  }
  while (GLenum error = glGetError()) {
    LOG_S(ERROR) << "OpenGL error: " << glErrorToString(error) << " in file " << file << ":" << line << " in " << function;
    //throw std::runtime_error("OpenGL error: " + glErrorToString(error));
  }
  return true;
}

/**
 * @brief clears opengl errors
 **/
void glClearErrors() {
  while (glGetError() != GL_NO_ERROR)
    ;
}

/**
 * @brief translates std::vector<float> to std::vector<glm::vec3>
 * @param arrayOfFloats
 * @return returns empty array on error
 */
std::vector<glm::vec3> floatArrayToVec3Array(std::vector<float> arrayOfFloats) {
  if (arrayOfFloats.size() % 3 != 0) {
    LOG_S(ERROR) << "Cannot perform operation arrayOfFloats is not multiple of 3 (it's size is:" << arrayOfFloats.size() << ")";
    return {};
  }
  std::vector<glm::vec3> vec3Array;
  for (int i = 0; i < arrayOfFloats.size() - 1; i += 3) {
    vec3Array.emplace_back(arrayOfFloats[i], arrayOfFloats[i + 1], arrayOfFloats[i + 2]);
  }
  return vec3Array;
}

std::vector<float> vec3ArrayToFloatArray(std::vector<glm::vec3> arrayOfVec3) {
  std::vector<float> floatArray{};
  for (auto &vec3Element : arrayOfVec3) {
    floatArray.push_back(vec3Element.x);
    floatArray.push_back(vec3Element.y);
    floatArray.push_back(vec3Element.z);
  }
  return floatArray;
}

void debugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                  const GLchar *message, const void *userParam) {
  // Print, log, whatever based on the enums and message
}

void logInitWin([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
#if defined(__WIN32__)
  el::Configurations defaultConf;
  defaultConf.setToDefault();
  // Values are always std::string
  defaultConf.set(el::Level::Info,el::ConfigurationType::Format, "%datetime %level %msg");
  // To set GLOBAL configurations you may use
  defaultConf.setGlobally(
      el::ConfigurationType::Format, "%datetime %thread %fbase:%line  %level| %msg");
  el::Loggers::reconfigureLogger("default", defaultConf);
#endif
}

void logInit([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
  if (isWindows()) {
    logInitWin(argc, argv);
    return;
  }
#if not defined(__WIN32__)
  loguru::init(argc, argv);

  // Put every log message in "everything.log":
  loguru::add_file("main.log", loguru::Truncate, loguru::Verbosity_MAX);
#endif
}
#endif// CG_LABS_FUNCTIONS_HPP