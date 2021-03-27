
#ifndef CGLABS__BUFFER_HPP_
#define CGLABS__BUFFER_HPP_

#include "functions.hpp"
class Buffer {
 public:
  enum type {
	VERTEX,
	NORMAL,
	TEXTURE_COORDS,
	COLOR,
	INDEX,
	OTHER
  };

 public:
  unsigned int rendererID{};
  void bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, rendererID);
  }
  [[maybe_unused]] static void unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
  explicit Buffer(std::vector<float> points) {
	glGenBuffers(1, &rendererID);
	glBindBuffer(GL_ARRAY_BUFFER, rendererID);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(), GL_STATIC_DRAW);
  }
  Buffer(std::vector<float> points, int attributePosition) {
	glGenBuffers(1, &rendererID);
	glBindBuffer(GL_ARRAY_BUFFER, rendererID);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(), GL_STATIC_DRAW);
	attributeLocation = attributePosition;
  }
  type bufferType{OTHER};
  int attributeLocation{0};
};

#endif//CGLABS__BUFFER_HPP_