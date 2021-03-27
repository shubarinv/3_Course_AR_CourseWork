
#ifndef CGLABS__VERTEX_ARRAY_HPP_
#define CGLABS__VERTEX_ARRAY_HPP_

#include "../functions.hpp"
#include "vertex_buffer.hpp"
#include "vertex_buffer_layout.hpp"
class VertexArray {
 private:
  unsigned int rendererID{};

 public:
  VertexArray() {
	glCall(glGenVertexArrays(1, &rendererID));
	LOG_S(INFO) << "VertexArray created rendererID: " << rendererID;
  }
  ~VertexArray() {
	glCall(glDeleteVertexArrays(1, &rendererID));
	LOG_S(INFO) << "VertexArray destroyed rendererID: " << rendererID;
  }
  void bind() const {
	glCall(glBindVertexArray(rendererID));
  }
  [[maybe_unused]] static void unbind() {
	glCall(glBindVertexArray(0));
  }
  void addBuffer(const Buffer &buffer, const VertexBufferLayout &layout, int vertexAttribIndex = 0) const {
	if(layout.getElements().empty()){
	  LOG_S(WARNING) << "Hey! you are trying to add empty layout, are you sure you meant to do it?";
	}
	unsigned int offset{0};
	bind();
	buffer.bind();
	const auto &elements = layout.getElements();
	for (auto element : elements) {
	  glCall(glVertexAttribPointer(vertexAttribIndex, element.length, element.type, element.normalized,
								   layout.getStride(), (const void *)offset));
	  offset += element.length * VertexBufferElement::getSize(element.type);
	}
	glCall(glEnableVertexAttribArray(vertexAttribIndex));
  }

  [[deprecated]][[maybe_unused]] void addLayout(VertexBufferElement layout) {
  }
};

#endif//CGLABS__VERTEX_ARRAY_HPP_