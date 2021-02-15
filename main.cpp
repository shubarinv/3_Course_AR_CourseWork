
#include <glm/gtx/color_space.hpp>
#include <random>

#include "application.hpp"
#include "camera.hpp"
#include "cube_map_texture.hpp"
#include "lights_manager.hpp"
#include "mesh.hpp"
#include "plane.h"
#include "renderer.hpp"
#include "shader.hpp"

LightsManager *lightsManager;
float lastX = 0;
float lastY = 0;
bool firstMouse = true;
// timing
double deltaTime = 0.0f;// time between current frame and last frame
double lastFrame = 0.0f;
Camera *camera;
int pressedKey = -1;

template<typename Numeric, typename Generator = std::mt19937>
Numeric random(Numeric from, Numeric to) {
  thread_local static Generator gen(std::random_device{}());

  using dist_type = typename std::conditional<
	  std::is_integral<Numeric>::value, std::uniform_int_distribution<Numeric>, std::uniform_real_distribution<Numeric> >::type;

  thread_local static dist_type dist;

  return dist(gen, typename dist_type::param_type{from, to});
}

std::vector<glm::vec3> getCoordsForVertices(double xc, double yc, double size, int n) {
  std::vector<glm::vec3> vertices;
  auto xe = xc + size;
  auto ye = yc;
  vertices.emplace_back(xe, yc, ye);
  double alpha = 0;
  for (int i = 0; i < n - 1; i++) {
	alpha += 2 * M_PI / n;
	auto xr = xc + size * cos(alpha);
	auto yr = yc + size * sin(alpha);
	xe = xr;
	ye = yr;
	vertices.emplace_back(xe, yc, ye);
  }
  return vertices;
}

void programQuit([[maybe_unused]] int key, [[maybe_unused]] int action, Application *app) {
  app->close();
  LOG_S(INFO) << "Quiting...";
}

void wasdKeyPress([[maybe_unused]] int key, [[maybe_unused]] int action, [[maybe_unused]] Application *app) {
  if (action == GLFW_PRESS) { pressedKey = key; }
  if (action == GLFW_RELEASE) { pressedKey = -1; }
}

void moveCamera() {
  if (pressedKey == GLFW_KEY_W) { camera->ProcessKeyboard(FORWARD, (float)deltaTime); }
  if (pressedKey == GLFW_KEY_S) { camera->ProcessKeyboard(BACKWARD, (float)deltaTime); }
  if (pressedKey == GLFW_KEY_A) { camera->ProcessKeyboard(LEFT, (float)deltaTime); }
  if (pressedKey == GLFW_KEY_D) { camera->ProcessKeyboard(RIGHT, (float)deltaTime); }
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (firstMouse) {
	lastX = (float)xpos;
	lastY = (float)ypos;
	firstMouse = false;
  }

  double xoffset = xpos - lastX;
  double yoffset = lastY - ypos;// reversed since y-coordinates go from bottom to top

  lastX = (float)xpos;
  lastY = (float)ypos;

  camera->ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera->ProcessMouseScroll(yoffset);
}
void renderScene(Shader *shader, std::vector<Mesh *> meshes, std::vector<Plane *> planes, bool shadowPass = true);
void renderQuad(Shader *shader);

int main(int argc, char *argv[]) {
  Application app({1280, 720}, argc, argv);
  Application::setOpenGLFlags();
  app.registerKeyCallback(GLFW_KEY_ESCAPE, programQuit);

  app.registerKeyCallback(GLFW_KEY_W, wasdKeyPress);
  app.registerKeyCallback(GLFW_KEY_A, wasdKeyPress);
  app.registerKeyCallback(GLFW_KEY_S, wasdKeyPress);
  app.registerKeyCallback(GLFW_KEY_D, wasdKeyPress);

  lastX = app.getWindow()->getWindowSize().x / 2.0f;
  lastY = app.getWindow()->getWindowSize().y / 2.0f;

  // shadows stuff
  // configure depth map FBO
  // -----------------------
  const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
  unsigned int depthMapFBO;
  glGenFramebuffers(1, &depthMapFBO);
  // create depth texture
  unsigned int depthMap;
  glGenTextures(1, &depthMap);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // attach depth texture as FBO's depth buffer
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // end of shadows related stuff
  glCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

  Shader shader_shadow("shaders/shadow_shader.glsl", false);
  ;

  Shader shader_tex("shaders/lighting_shader.glsl", false);
  shader_tex.bind();
  shader_tex.setUniform1i("skybox", 0);
  shader_tex.setUniform1i("shadowMap", 1);
  shader_tex.setUniform1i("NUM_POINT_LIGHTS", 0);
  shader_tex.setUniform1i("NUM_SPOT_LIGHTS", 0);
  shader_tex.setUniform1i("NUM_DIR_LIGHTS", 0);

  Shader shader_skybox("shaders/skybox_shader.glsl");
  shader_skybox.bind();
  shader_skybox.setUniform1i("skybox", 0);
  shader_skybox.setUniform1f("intensity", 0.3);
  ObjLoader objLoader;
  std::vector<Mesh *> meshes;

  std::vector<Plane *> planes;
  float skyboxVertices[] = {
	  // positions
	  -1.0f, 1.0f, -1.0f,
	  -1.0f, -1.0f, -1.0f,
	  1.0f, -1.0f, -1.0f,
	  1.0f, -1.0f, -1.0f,
	  1.0f, 1.0f, -1.0f,
	  -1.0f, 1.0f, -1.0f,

	  -1.0f, -1.0f, 1.0f,
	  -1.0f, -1.0f, -1.0f,
	  -1.0f, 1.0f, -1.0f,
	  -1.0f, 1.0f, -1.0f,
	  -1.0f, 1.0f, 1.0f,
	  -1.0f, -1.0f, 1.0f,

	  1.0f, -1.0f, -1.0f,
	  1.0f, -1.0f, 1.0f,
	  1.0f, 1.0f, 1.0f,
	  1.0f, 1.0f, 1.0f,
	  1.0f, 1.0f, -1.0f,
	  1.0f, -1.0f, -1.0f,

	  -1.0f, -1.0f, 1.0f,
	  -1.0f, 1.0f, 1.0f,
	  1.0f, 1.0f, 1.0f,
	  1.0f, 1.0f, 1.0f,
	  1.0f, -1.0f, 1.0f,
	  -1.0f, -1.0f, 1.0f,

	  -1.0f, 1.0f, -1.0f,
	  1.0f, 1.0f, -1.0f,
	  1.0f, 1.0f, 1.0f,
	  1.0f, 1.0f, 1.0f,
	  -1.0f, 1.0f, 1.0f,
	  -1.0f, 1.0f, -1.0f,

	  -1.0f, -1.0f, -1.0f,
	  -1.0f, -1.0f, 1.0f,
	  1.0f, -1.0f, -1.0f,
	  1.0f, -1.0f, -1.0f,
	  -1.0f, -1.0f, 1.0f,
	  1.0f, -1.0f, 1.0f};
  // skybox VAO
  unsigned int skyboxVAO, skyboxVBO;
  glGenVertexArrays(1, &skyboxVAO);
  glGenBuffers(1, &skyboxVBO);
  glBindVertexArray(skyboxVAO);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  // load textures
  // -------------

  std::vector<std::string> faces{
	  "textures/skybox/right.jpg",
	  "textures/skybox/left.jpg",
	  "textures/skybox/top.jpg",
	  "textures/skybox/bottom.jpg",
	  "textures/skybox/front.jpg",
	  "textures/skybox/back.jpg"};
  unsigned int cubemapTexture = CubeMapTexture::loadCubemap(faces);

  lightsManager = new LightsManager;
  lightsManager->addLight(LightsManager::DirectionalLight("sun", {0, -5, -15}, {0.1, 0.1, 0.1}, {1, 0.9, 0.7}, {1, 1, 1}));
  //lightsManager->addLight(LightsManager::SpotLight("sun",{-2.0f, 0.0f, -1.0f},{0,0,0},{0.1,0.1,0.1},{1,1,1},{1,1,1},glm::cos(glm::radians(180.f)),180,1.0f,0.09f,0.032f));

  // camera
  camera = new Camera(glm::vec3(-95, 0, 35));
  camera->setWindowSize(app.getWindow()->getWindowSize());

  glfwSetCursorPosCallback(app.getWindow()->getGLFWWindow(), mouse_callback);
  glfwSetScrollCallback(app.getWindow()->getGLFWWindow(), scroll_callback);

  meshes.push_back(new Mesh("resources/models/Starship.obj"));
  meshes.back()->addTexture("textures/Starship_Base_Color.png")->compile();
  meshes.back()->setScale({0.005, 0.005, 0.005})->compile();
  meshes.back()->setPosition({-95, -5, 15});
  meshes.push_back(new Mesh("resources/models/pahrump_print_1x.obj"));
  meshes.back()->addScaledTexture("textures/marsTexture.jpg", {2, 2});
  meshes.back()->setScale({1, 1, 1})->compile();
  meshes.back()->setPosition({0, -20, 0});

  meshes.push_back(new Mesh("resources/models/HDU_lowRez_part1.obj"));
  meshes.back()->compile()->setPosition({-150, -6, 25})->setScale({0.01, 0.01, 0.01});
  meshes.push_back(new Mesh("resources/models/HDU_lowRez_part1.obj"));
  meshes.back()->compile()->setPosition({-150, -6, 35.3})->setOrigin({-150, -6, 35.3})->setScale({0.01, 0.01, 0.01})->setRotation({0, 90, 0});
  meshes.push_back(new Mesh("resources/models/HDU_lowRez_part2.obj"));
  meshes.back()->compile()->setPosition({-150, -6, 25})->setScale({0.01, 0.01, 0.01});
  meshes.push_back(new Mesh("resources/models/Cartoon Low pOly Solar Panel.obj"));
  meshes.back()->compile()->setPosition({-160, -4, 32})->setOrigin({-160, -4, 32})->setScale({0.01, 0.01, 0.01})->setRotation({0, 90, 0});
  meshes.push_back(new Mesh("resources/models/Cartoon Low pOly Solar Panel.obj"));
  meshes.back()->compile()->setPosition({-140, -4, 27})->setOrigin({-140, -4, 27})->setScale({0.01, 0.01, 0.01})->setRotation({0, 90, 0});

  double lasttime = glfwGetTime();
  while (!app.getShouldClose()) {
	app.getWindow()->updateFpsCounter();
	auto currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	moveCamera();
	Renderer::clear({0, 0, 0, 1});

	// 1. render depth of scene to texture (from light's perspective)
	// --------------------------------------------------------------
	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 0.001f, far_plane = 30.5f;
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	lightView = glm::lookAt(glm::vec3(-85, 10, 25),
							glm::vec3(-95, 0, 15),
							glm::vec3(0.0f, 1.0f, 0.0f));
	lightSpaceMatrix = lightProjection * lightView;
	// render scene from light's point of view
	shader_shadow.bind();

	shader_shadow.setUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);
	glm::mat4 model = glm::mat4(1.0f);
	shader_shadow.setUniformMat4f("model", model);

	glViewport(0, 0, 1024, 1024);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	renderScene(&shader_shadow, meshes, planes);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 2. render scene as normal using the generated depth/shadow map
	// --------------------------------------------------------------
	glViewport(0, 0, app.getWindow()->getWindowSize().x, app.getWindow()->getWindowSize().y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader_tex.bind();
	camera->passDataToShader(&shader_tex);
	lightsManager->passDataToShader(&shader_tex);
	shader_tex.setUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	renderScene(&shader_tex, meshes, planes, false);
	/*
	shader_depth_debug.bind();
	shader_depth_debug.setUniform1f("near_plane", near_plane);
	shader_depth_debug.setUniform1f("far_plane", far_plane);
	camera->passDataToShader(&shader_depth_debug);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	renderQuad(&shader_depth_debug);

	LOG(INFO) << "x: " << camera->Position.x << "y: " << camera->Position.y << "z: " << camera->Position.z;
*/
	// draw skybox as last

	glDepthFunc(GL_LEQUAL);// change depth function so depth test passes when values are equal to depth buffer's content
	shader_skybox.bind();
	shader_skybox.setUniform1f("intensity", 1);
	auto view = glm::mat4(glm::mat3(camera->GetViewMatrix()));// remove translation from the view matrix
	shader_skybox.setUniformMat4f("view", view);
	shader_skybox.setUniformMat4f("projection", camera->getProjection());
	// skybox cube
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);// set depth function back to default

	glCall(glfwSwapBuffers(app.getWindow()->getGLFWWindow()));
	glfwPollEvents();
	while (glfwGetTime() < lasttime + 1.0 / 60) {
	  // TODO: Put the thread to sleep, yield, or simply do nothing
	}
	lasttime += 1.0 / 60;
  }
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
void renderScene(Shader *shader, std::vector<Mesh *> meshes, std::vector<Plane *> planes, bool shadowPass) {
  if (!shadowPass) {
	camera->passDataToShader(shader);
	lightsManager->passDataToShader(shader);
  }

  //plane.draw(&shader);
  for (auto &plane : planes) {
	plane->draw(shader);
  }
  for (auto &mesh : meshes) {
	mesh->draw(shader);
  }
}
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad(Shader *shader) {
  shader->bind();
  if (quadVAO == 0) {
	float quadVertices[] = {
		// positions        // texture Coords
		-1.0f,
		1.0f,
		0.0f,
		0.0f,
		1.0f,
		-1.0f,
		-1.0f,
		0.0f,
		0.0f,
		0.0f,
		1.0f,
		1.0f,
		0.0f,
		1.0f,
		1.0f,
		1.0f,
		-1.0f,
		0.0f,
		1.0f,
		0.0f,
	};
	// setup plane VAO
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
  }
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}
