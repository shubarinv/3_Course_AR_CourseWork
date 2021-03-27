
#ifndef CGLABS__COLOR_BUFFER_HPP_
#define CGLABS__COLOR_BUFFER_HPP_
#include "buffer.hpp"
#include "functions.hpp"
class ColorBuffer : public Buffer {

 public:
  /**
   * @brief Holds data about vertices
   * @param points std::vector<float>
   */
  explicit ColorBuffer(std::vector<float> points) : Buffer(std::move(points)) {
	bufferType = Buffer::type::COLOR;
	attributeLocation=1;
  }
  explicit ColorBuffer(std::vector<glm::vec3> coordinates) : Buffer(std::move(vec3ArrayToFloatArray(std::move(coordinates)))) {
	bufferType = Buffer::type::COLOR;
	attributeLocation=1;
  }
};

#endif//CGLABS__COLOR_BUFFER_HPP_
