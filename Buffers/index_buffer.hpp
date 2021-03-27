

#ifndef CGLABS__INDEX_BUFFER_HPP_
#define CGLABS__INDEX_BUFFER_HPP_

#include "../buffer.hpp"
#include "../functions.hpp"
class IndexBuffer {

 public:
  /**
   * @brief Holds data about vertices
   * @param points std::vector<float>
   */
   uint rendererID{};
  explicit IndexBuffer(const std::vector<unsigned int>& indices){
	glCall(glGenBuffers(1, &rendererID));
	glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID));
	glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW));
  }
  void bind() const {
	glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID));
  }
  [[maybe_unused]] static void unbind() {
	glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
  }

};
#endif//CGLABS__INDEX_BUFFER_HPP_
