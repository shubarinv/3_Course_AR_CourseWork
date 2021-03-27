
#ifndef CGLABS__WINDOW_HPP_
#define CGLABS__WINDOW_HPP_
#include "functions.hpp"
class Window {
 private:
  GLFWwindow *window= nullptr; ///< reference to glfw window
  glm::vec2 windowSize{};

 public:
  /**
   * @brief returns window size
   * @return vec2<int>
   */
  [[maybe_unused]] [[nodiscard]] const glm::vec2 &getWindowSize() const {
	return windowSize;
  }
/**
 * @brief returns reference to glfwWindow
 * @return GLFWwindow *
 */
  [[nodiscard]] GLFWwindow *getGLFWWindow() const {
	return window;
  }

  /**
   * @brief init for window class
   * @param size vec2<int>window size
   */
  explicit Window(glm::vec2 size) {
	windowSize = size;
	// GLFW lib init
	glfwSetErrorCallback(glfwErrorHandler);
	if (!glfwInit()) {
	  LOG_S(FATAL) << "GLFW INIT - FAIL";
	  throw std::runtime_error("Failed to init glfw");
	}
	LOG_S(INFO) << "GLFW init - OK";

	if (isMac()) {
	  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	  LOG_S(INFO) << "System: MacOS";
	}
	glfwWindowHint(GLFW_FLOATING, GL_TRUE);
	glfwWindowHint(GLFW_FOCUS_ON_SHOW, GL_TRUE);
	// GLFW Window creation
	bruteforceGLVersion();
	glfwMakeContextCurrent(window);
	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (window == nullptr) {
	  LOG_S(FATAL) << "GLFW was unable to create window";
	  glfwTerminate();
	}
	// GLAD lib init
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
	  LOG_S(FATAL) << "GLAD init Failed";
	}

	GLint maxShaderTextures;
	GLint maxTotalTextures;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxShaderTextures);
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTotalTextures);
	glEnable(GL_MULTISAMPLE);
	LOG_S(INFO) << "Status: Using OpenGL v" << glGetString(GL_VERSION);
	LOG_S(INFO) << "Renderer: " << glGetString(GL_RENDERER); /* get renderer string */
	LOG_S(INFO) << "Number of textures that can be accessed by the fragment shader: " << maxShaderTextures;
	LOG_S(INFO) << "Total number of textures that can be used " << maxTotalTextures;
	LOG_S(INFO) << "Init DONE!";
  }
  static void glfwErrorHandler(int error, const char *message) {
	if (error != 65543)
	  LOG_S(ERROR) << "GLFW error: " << error << " " << message;
  }
  ~Window() {
	glfwDestroyWindow(window);
	LOG_S(INFO) << "GLFW window destroyed";
	LOG_S(INFO) << "Window(" << this << ") destroyed";
  }
  [[maybe_unused]] void updateFpsCounter() {
	static double previous_seconds = glfwGetTime();
	static int frame_count;
	double current_seconds = glfwGetTime();
	double elapsed_seconds = current_seconds - previous_seconds;
	if (elapsed_seconds > 0.25) {
	  previous_seconds = current_seconds;
	  double fps       = (double)frame_count / elapsed_seconds;
	  std::string tmp  =  "Marcusessssss courseWork @ fps: " + std::to_string(fps);
	  glfwSetWindowTitle(window, tmp.c_str());
	  frame_count = 0;
	}
	frame_count++;
  }
 private:
  /**
   * @brief this function attempts to load latest version of OpenGL
   */
  void bruteforceGLVersion() {
	LOG_S(INFO)<<"Getting latest OpenGL";
	for (int major = 4; major > 2; major--) {
	  for (int minor = 9; minor > 0; minor--) {
		if (major == 3 & minor == 1) { return; }
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
		std::string tmp = "Trying OpenGL v" + std::to_string(major) + "." + std::to_string(minor);
		window = glfwCreateWindow(windowSize.x, windowSize.y, "UNSET", nullptr, nullptr);
		if (window == nullptr) {
		  LOG_S(INFO) << tmp << " - FAIL";
		} else {
		  LOG_S(INFO) << tmp << " - OK";
		  return;
		}
	  }
	}
  }
};

#endif//CGLABS__WINDOW_HPP_