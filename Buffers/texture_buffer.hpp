
#ifndef CGLABS__TEXTURE_BUFFER_HPP_
#define CGLABS__TEXTURE_BUFFER_HPP_
#include "../buffer.hpp"
#include "../functions.hpp"
class TextureBuffer : public Buffer {

 public:
  /**
   * @brief Holds data about vertices
   * @param points std::vector<float>
   */
  explicit TextureBuffer(std::vector<float> points) : Buffer(std::move(points)) {
	bufferType = Buffer::type::TEXTURE_COORDS;
	attributeLocation=2;
  }
  explicit TextureBuffer(std::vector<glm::vec3> coordinates) : Buffer(std::move(vec3ArrayToFloatArray(std::move(coordinates)))) {
	bufferType = Buffer::type::TEXTURE_COORDS;
	attributeLocation=2;
  }
};

#endif//CGLABS__TEXTURE_BUFFER_HPP_
