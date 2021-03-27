
#ifndef CGLABS__VERTEX_BUFFER_LAYOUT_HPP_
#define CGLABS__VERTEX_BUFFER_LAYOUT_HPP_

#include <vector>

struct VertexBufferElement {
  unsigned int type;
  unsigned int length;
  unsigned char normalized;

  static unsigned int getSize(unsigned int type) {
	switch (type) {
	  case GL_FLOAT:
	  case GL_UNSIGNED_INT: return 4;
	  case GL_UNSIGNED_BYTE: return 1;
	  default: return -1;
	}
  }
};
class VertexBufferLayout {
  std::vector<VertexBufferElement> elements;
  unsigned int stride{0};

 public:
  [[maybe_unused]] [[nodiscard]] inline std::vector<VertexBufferElement> getElements() const {
	return elements;
  }
  [[nodiscard]] inline unsigned int getStride() const {
	return stride;
  }
  template<typename T>
  void push([[maybe_unused]] unsigned int length) {
  }

};
template<>
void VertexBufferLayout::push<float>(unsigned int length) {
  elements.push_back({GL_FLOAT, length, GL_FALSE});
  stride += length * VertexBufferElement::getSize(GL_FLOAT);
}
template<>
void VertexBufferLayout::push<unsigned int>(unsigned int length) {
  elements.push_back({GL_UNSIGNED_INT, length, GL_FALSE});
  stride += length * VertexBufferElement::getSize(GL_UNSIGNED_INT);
}
template<>
void VertexBufferLayout::push<unsigned char>(unsigned int length) {
  elements.push_back({GL_UNSIGNED_BYTE, length, GL_FALSE});
  stride += length * VertexBufferElement::getSize(GL_UNSIGNED_BYTE);
}

#endif//CGLABS__VERTEX_BUFFER_LAYOUT_HPP_