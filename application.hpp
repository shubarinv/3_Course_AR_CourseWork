
#ifndef CGLABS__APPLICATION_HPP_
#define CGLABS__APPLICATION_HPP_

#include <map>

#include "window.hpp"

class Application {
 private:
  Window *window{}; ///< Reference to class that creates OpenGL window
  typedef void (*functionType)(int, int, Application *); ///< typedef for a function like void fun(int key, int action, Application *app)
  std::map<int, functionType> keyPressCallbacks;///< Holds registered callbacks
  bool shouldClose{false}; ///< whether window should close
 public:

  /**
   * @brief initialises application
   * @param windowSize glm::vec2 window size
   * @param argc used by logging lib
   * @param argv used by logging lib
   */
  void init(glm::vec2 windowSize, [[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
	LOG_S(INFO) << "Hello world!";
	logInit(argc, argv);
	window = new Window(windowSize);
	setOpenGLFlags();
	/// following is required for keyboard related callbacks
	glfwSetKeyCallback(window->getGLFWWindow(), keyCallback);
	glfwSetWindowUserPointer(window->getGLFWWindow(), this);
  }
  Application(glm::vec2 windowSize, [[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
	init(windowSize, argc, argv);
  }
  /**
   * @brief registers callback for keypress
   * @param key GLFW_key you want to register callback for
   * @param func ref to function that you want to be called
   * @example app.registerKeyCallback(GLFW_KEY_ESCAPE, programQuit);
   */
  void registerKeyCallback(int key, functionType func) {
	auto posOfPreviouslyRegisteredKeyPressCallbacks = keyPressCallbacks.find(key);
	if (posOfPreviouslyRegisteredKeyPressCallbacks != keyPressCallbacks.end()) {
	  LOG_S(INFO) << "Callback for \"" << glfwGetKeyName(key, 0) << "\" was already registered";
	}
	keyPressCallbacks.emplace(key, func);
	LOG_S(INFO) << "New callback was registered";
  }

  /**
   * @brief removes callback to a specified key
   * @param key GLFW_key for which you want to remove callback
   */
  [[maybe_unused]] void unregisterKeyCallback(int key) {
	auto posOfPreviouslyRegisteredKeyPressCallbacks = keyPressCallbacks.find(key);
	if (posOfPreviouslyRegisteredKeyPressCallbacks != keyPressCallbacks.end()) {
	  keyPressCallbacks.erase(posOfPreviouslyRegisteredKeyPressCallbacks);
	}
  }

  /**
   * @brief sets opengl flags
   */
  [[maybe_unused]] static void setOpenGLFlags() {
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  /**
   * @brief returns reference to Window class
   * @return Window*
   */
  [[nodiscard]] Window *getWindow() const {
	return window;
  }
  /**
   * @brief Closes window and cleans up
   */
  void close() {
	shouldClose = true;
	delete (window);
  }

  ~Application() {
	LOG_S(INFO) << "Application destroyed";
  }
/**
 * @brief whether window should close
 * @return bool: true if window should close, false - otherwise
 */
  [[nodiscard]] bool getShouldClose() const {
	return shouldClose;
  }
 private:
  /**
   * @brief handles key presses
   * @param _window
   * @param key
   * @param scancode
   * @param action
   * @param mods
   */
  void handleKeyboard([[maybe_unused]] GLFWwindow *_window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods) {
	auto requiredCallback = keyPressCallbacks.find(key);
	if (requiredCallback != keyPressCallbacks.end()) {
	  LOG_S(INFO) << "Calling registered callback";
	  requiredCallback->second(key, action, this);
	}
  }

  static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	auto *app = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));
	app->handleKeyboard(window, key, scancode, action, mods);
  }

};

#endif //CGLABS__APPLICATION_HPP_