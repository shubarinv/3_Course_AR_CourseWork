
#ifndef CGLABS__RENDERER_HPP_
#define CGLABS__RENDERER_HPP_

#include <string>

#include "Buffers/vertex_array.hpp"
#include "shader.hpp"

class Renderer {
 public:
  /**
   * @brief Draws on screen
   * @param vertexArray reference to vertex array of an object you want to draw.
   * @param indexBuffer reference to index buffer of an object you want to draw.
   * @param shader  reference to shader that will be used.
   * @param mode draw mode. (GL_TRIANGLES, GL_LINES, GL_TRIANGLE_STRIP, ...)
   */
  /*
static void draw(VertexArray *vertexArray, IndexBuffer *indexBuffer, Shader *shader, GLuint mode = GL_TRIANGLES) {
  shader->bind();
  vertexArray->bind();
  indexBuffer->bind();
  glCall(glDrawElements(mode, indexBuffer->getLength(), GL_UNSIGNED_INT, nullptr));
}*/
  /**
 * @brief Draws object on screen.
 * @param object reference to object you want to draw.
 * @param shader reference to shader you want to use.
 * @param mode draw mode. (GL_TRIANGLES, GL_LINES, GL_TRIANGLE_STRIP, ...)
 */
  /*
 static void draw(Object *object, Shader *shader, GLuint mode = GL_TRIANGLES) {
   object->draw();
   shader->bind();
   object->getVertexArray()->bind();
   object->getIndexBuffer()->bind();
   glCall(glDrawElements(mode, object->getIndexBuffer()->getLength(), GL_UNSIGNED_INT, nullptr));
 }
 */
  /**
   * @brief Clears screen with specified color.
   * @param clearColor color with which to clear screen
   */
  static void clear(glm::vec4 clearColor = {0, 0, 0, 0}) {
	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
  static void draw(VertexArray *vao, Shader *shader, unsigned long range, GLuint mode = GL_TRIANGLES) {
	vao->bind();
	shader->bind();
	glCall(glDrawArrays(mode, 0, range));
  }
  static void draw(IndexBuffer *ibo,VertexArray*vao, Shader *shader,unsigned long range, GLuint mode = GL_TRIANGLES) {
	vao->bind();
	ibo->bind();
	shader->bind();
	glCall(glDrawElements(mode,range, GL_UNSIGNED_INT, nullptr));
  }
};

#endif//CGLABS__RENDERER_HPP_