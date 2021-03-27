

#ifndef CGLABS__NORMALS_BUFFER_HPP_
#define CGLABS__NORMALS_BUFFER_HPP_
#include "../buffer.hpp"
#include "../functions.hpp"
class NormalsBuffer : public Buffer {

 public:
  /**
   * @brief Holds data about vertices
   * @param points std::vector<float>
   */
  explicit NormalsBuffer(std::vector<float> points) : Buffer(std::move(points)) {
	bufferType = Buffer::type::NORMAL;
	attributeLocation=3;
  }
  explicit NormalsBuffer(std::vector<glm::vec3> coordinates) : Buffer(std::move(vec3ArrayToFloatArray(std::move(coordinates)))) {
	bufferType = Buffer::type::NORMAL;
	attributeLocation=3;
  }
};


#endif//CGLABS__NORMALS_BUFFER_HPP_
