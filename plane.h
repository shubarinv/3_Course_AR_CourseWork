
#ifndef CGCOURSEWORK_PLANE_H
#define CGCOURSEWORK_PLANE_H

#include <glm/gtx/normal.hpp>

#include "functions.hpp"
#include "renderer.hpp"

class Plane {
  std::vector<Buffer> buffers{};
  std::vector<Texture *> textures{};
  VertexArray *vao{nullptr};
  glm::mat4 model{};
  glm::vec3 position{0, 0, 0};
  glm::vec3 origin{0, 0, 0};
  glm::vec3 rotation{0, 0, 0};
  glm::vec3 scale{1, 1, 1};
  glm::vec2 texScale{1, 1};

 public:
  [[nodiscard]] const glm::vec3 &getPosition() const {
	return position;
  }

  [[nodiscard]] const glm::vec3 &getOrigin() const {
	return origin;
  }

  [[nodiscard]] const glm::vec3 &getRotation() const {
	return rotation;
  }

  [[nodiscard]] const glm::vec3 &getScale() const {
	return scale;
  }

 private:
  std::vector<float> coordinates;
  bool texCoordsIgnoreScale = false;

 public:
  Plane(glm::vec3 a1, glm::vec3 a2, glm::vec3 b1, glm::vec3 b2, glm::vec3 scale = {1, 1, 1}, bool _texCoordsIgnoreScale = false) {
	coordinates = vec3ArrayToFloatArray({a1, a2, b1, b1, b2, a1});

	vao = new VertexArray;
	model = glm::mat4(1.f);
	setScale(scale);
	texCoordsIgnoreScale = _texCoordsIgnoreScale;
  }
  Plane(glm::vec3 a1, glm::vec3 a2, glm::vec3 b1, glm::vec3 b2, glm::vec3 scale = {1, 1, 1}, glm::vec2 _texScale = {1, 1}) {
	coordinates = vec3ArrayToFloatArray({a1, a2, b1, b1, b2, a1});

	vao = new VertexArray;
	model = glm::mat4(1.f);
	setScale(scale);
	texScale = _texScale;
	texCoordsIgnoreScale = true;
  }
  Plane *compile() {
	if (coordinates.empty()) {
	  LOG_S(ERROR) << "Coordinates were not set!";
	  return this;
	}
	addNewBuffer(VertexBuffer(coordinates));// Setting VBO
	generateNormals();
	if (textures.size() == 1) {
	  addTexture("textures/NoSpec.png");
	}
	fillVAO();

	return this;
  }

  Plane *draw(Shader *shader) {
	shader->setUniformMat4f("model", model);

	shader->setUniform1f("material.shininess", 10);

	if (!textures.empty()) {
	  shader->setUniform1i("material.diffuse", 0);
	  if (textures.size() == 2) {
		shader->setUniform1i("material.specular", 1);
	  }
	  for (int i = 0; i < textures.size(); ++i) {
		textures[i]->bind(i);
	  }
	}

	Renderer::draw(vao, shader, coordinates.size() / 3, GL_TRIANGLES);
	return this;
  }

  Plane *setColor(glm::vec3 color) {
	std::vector<float> colors;
	for (auto &coord : coordinates) {
	  colors.push_back(color.r);
	  colors.push_back(color.g);
	  colors.push_back(color.b);
	}
	addNewBuffer(ColorBuffer(colors), true);
	return this;
  }

  Plane *addTexture(const std::string &filePath) {
	textures.push_back(new Texture(filePath));
	if (!wasBufferDefined(Buffer::TEXTURE_COORDS)) {
	  LOG_S(INFO) << "Generating textureCoords";
	  if (texCoordsIgnoreScale) {
		auto texCoords = Texture::generateTextureCoords(coordinates.size() / 3, texScale);
		addNewBuffer(TextureBuffer(texCoords));
	  } else {
		auto texCoords = Texture::generateTextureCoords(coordinates.size() / 3, {scale.x / 2, scale.z / 2});
		addNewBuffer(TextureBuffer(texCoords));
	  }
	}
	return this;
  }

  Plane *setNormals(const std::vector<glm::vec3> &normals) {
	addNewBuffer(NormalsBuffer(normals), true);
	return this;
  }

  Plane *setNormals(const std::vector<float> &normals) {
	addNewBuffer(NormalsBuffer(normals), true);
	return this;
  }

  Plane *setTextureCoords(const std::vector<float> &textureCoords) {
	addNewBuffer(TextureBuffer(textureCoords), true);
	return this;
  }

 private:
  Plane *addNewBuffer(Buffer _buffer, bool bReplace = false) {
	bool wasReplaced = false;
	for (auto &buffer : buffers) {
	  if (_buffer.bufferType == buffer.bufferType && buffer.bufferType != Buffer::OTHER) {
		if (bReplace) {
		  buffer = _buffer;
		  wasReplaced = true;
		} else {
		  LOG_S(ERROR) << "Can't add buffer; buffer of same type already defined";
		  return this;
		}
	  }
	}
	if (!wasReplaced)
	  buffers.push_back(_buffer);
	return this;
  }

  void generateNormals() {
	auto vertices = floatArrayToVec3Array(coordinates);
	std::vector<glm::vec3> normals{};
	normals.reserve(6);
	for (int i = 0; i < 6; ++i) {
	  normals.push_back(glm::triangleNormal(vertices[0], vertices[1], vertices[2]) * glm::vec3(-1));
	}

	setNormals(normals);
  }

  Plane *fillVAO() {
	VertexBufferLayout layout3;
	VertexBufferLayout layout2;
	layout3.push<float>(3);///< number of params for each vertex
	layout2.push<float>(2);///< number of params for each vertex
	for (auto &buffer : buffers) {
	  if (buffer.bufferType != Buffer::type::INDEX && buffer.bufferType != Buffer::type::OTHER) {// We skip indexBuffer
		if (buffer.bufferType == Buffer::type::TEXTURE_COORDS) {
		  vao->addBuffer(buffer, layout2, buffer.attributeLocation);
		} else {
		  vao->addBuffer(buffer, layout3, buffer.attributeLocation);
		}
	  }
	}
	return this;
  }

  bool wasBufferDefined(Buffer::type bufferType) {
	for (auto &buffer : buffers) {
	  if (bufferType == buffer.bufferType) {
		return true;
	  }
	}
	return false;
  }

 public:
  Plane *setScale(glm::vec3 _scale) {
	scale = _scale;
	updateModel();
	return this;
  }

  Plane *setRotation(glm::vec3 _rotation) {
	rotation = _rotation;
	updateModel();
	return this;
  }

  Plane *setPosition(glm::vec3 _position) {
	position = _position;
	updateModel();
	return this;
  }

  Plane *setOrigin(glm::vec3 _origin) {
	origin = _origin;
	updateModel();
	return this;
  }

  Plane *updateModel() {
	model = glm::mat4(1.f);
	model = glm::translate(model, origin);
	model = glm::rotate(model, glm::radians(this->rotation.x), glm::vec3(1.f, 0.f, 0.f));
	model = glm::rotate(model, glm::radians(this->rotation.y), glm::vec3(0.f, 1.f, 0.f));
	model = glm::rotate(model, glm::radians(this->rotation.z), glm::vec3(0.f, 0.f, 1.f));
	model = glm::translate(model, position - origin);
	model = glm::scale(model, scale);
	return this;
  }

  std::vector<Texture *> getTextures() {
	return textures;
  }

  Plane *setTextures(std::vector<Texture *> _textures) {
	textures = std::move(_textures);
	if (!wasBufferDefined(Buffer::TEXTURE_COORDS)) {
	  LOG_S(INFO) << "Generating textureCoords";
	  auto texCoords = Texture::generateTextureCoords(coordinates.size() / 3);
	  addNewBuffer(TextureBuffer(texCoords));
	}
	return this;
  }
};

#endif//CGCOURSEWORK_PLANE_H
