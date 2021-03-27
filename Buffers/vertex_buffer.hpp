
#ifndef CGLABS__VERTEX_BUFFER_HPP_
#define CGLABS__VERTEX_BUFFER_HPP_
#include <utility>
#include <vector>

#include "../buffer.hpp"
#include "../functions.hpp"
class VertexBuffer : public Buffer {
 public:
  /**
   * @brief Holds data about vertices
   * @param points std::vector<float>
   */
  explicit VertexBuffer(std::vector<float> points) : Buffer(std::move(points)) {
	bufferType = Buffer::type::VERTEX;
  }
  explicit VertexBuffer(std::vector<glm::vec3> coordinates) : Buffer(std::move(vec3ArrayToFloatArray(std::move(coordinates)))) {
	bufferType = Buffer::type::VERTEX;
  }
};

#endif//CGLABS__VERTEX_BUFFER_HPP_
