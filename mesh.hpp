
#ifndef CGLABS__MESH_HPP_
#define CGLABS__MESH_HPP_

#include <utility>

#include "Buffers/index_buffer.hpp"
#include "Buffers/normals_buffer.hpp"
#include "Buffers/texture_buffer.hpp"
#include "Buffers/vertex_array.hpp"
#include "Buffers/vertex_buffer.hpp"
#include "color_buffer.hpp"
#include "functions.hpp"
#include "obj_loader.hpp"
#include "plane.h"
#include "renderer.hpp"
#include "texture.hpp"

class Mesh {
  std::vector<float> coordinates;

  glm::mat4 model{};

  std::vector<Buffer> buffers;
  std::vector<Texture *> textures;
  VertexArray *vao{nullptr};
  unsigned int indexBufferSize{0};
  IndexBuffer *indexBuffer{nullptr};
  std::vector<Mesh> relatedMeshes;

  Mesh() = default;

  explicit Mesh(std::vector<glm::vec3> _coordinates) {
	coordinates = vec3ArrayToFloatArray(std::move(_coordinates));
	vao = new VertexArray;
	model = glm::mat4(1.f);
  }

  Mesh *setColor(const std::vector<glm::vec3> &colorsArray) {
	if (colorsArray.size() * 3 != coordinates.size()) {
	  LOG_S(ERROR) << "Amount of elements in colorsArray(" << colorsArray.size() * 3
				   << ") doesn't match amount vertices<<coordinates<<. Will still try to set colors but this may cause problems";
	}
	addNewBuffer(ColorBuffer(colorsArray), true);
	return this;
  }

  Mesh *setColor(const std::vector<float> &colorsArray) {
	if (colorsArray.size() != coordinates.size()) {
	  LOG_S(ERROR) << "Amount of elements in colorsArray(" << colorsArray.size()
				   << ") doesn't match amount vertices<<coordinates<<. Will still try to set colors but this may cause problems";
	}
	addNewBuffer(ColorBuffer(colorsArray), true);
	return this;
  }

 public:
  glm::vec3 position{0, 0, 0};
  glm::vec3 origin{0, 0, 0};
  glm::vec3 rotation{0, 0, 0};
  glm::vec3 scale{1, 1, 1};

  unsigned long long int countVertices() {
	unsigned long long int vertCount = loadedOBJ.vertices.size();
	for (auto &mesh : relatedMeshes) {
	  vertCount += mesh.loadedOBJ.vertices.size();
	}
	return vertCount;
  }

  Mesh *draw(Shader *shader) {
	shader->bind();
	shader->setUniformMat4f("model", model);
	shader->setUniform1f("material.shininess", material.shininess);

	if (!textures.empty()) {
	  shader->setUniform1i("material.diffuse", 0);
	  shader->setUniform1i("useTexture", 1);
	  if (textures.size() == 2) {
		shader->setUniform1i("material.specular", 1);
	  }
	  for (int i = 0; i < textures.size(); ++i) {
		textures[i]->bind(i);
	  }
	} else {
	  shader->setUniform1i("useTexture", 0);
	  shader->setUniform3f("material.mat_diffuse", material.diffuse);
	  shader->setUniform3f("material.mat_specular", material.specular);
	  shader->setUniform3f("material.mat_ambient", material.ambient);
	}
	if (indexBuffer != nullptr) {
	  Renderer::draw(indexBuffer, vao, shader, indexBufferSize, GL_TRIANGLES);
	} else {
	  Renderer::draw(vao, shader, coordinates.size() / 3, GL_TRIANGLES);
	}
	for (auto &relatedMesh : relatedMeshes) {
	  relatedMesh.draw(shader);
	}
	return this;
  }

  explicit Mesh(std::vector<float> _coordinates) {
	coordinates = std::move(_coordinates);
	vao = new VertexArray;
	model = glm::mat4(1.f);
  }

  explicit Mesh(const std::string &filepath) {
	ObjLoader objLoader;
	auto meshes = objLoader.loadObj(filepath);
	loadedOBJ = meshes.front();
	coordinates = meshes.front().vertices;
	setTextureCoords(meshes.front().texCoords);
	setNormals(meshes.front().normals);
	vao = new VertexArray;
	material = loadedOBJ.material;
	for (int i = 1; i < meshes.size(); ++i) {
	  relatedMeshes.emplace_back(meshes[i]);
	  relatedMeshes.back().compile();
	}
	model = glm::mat4(1.f);
	setTextures(material.textures);
  }

  explicit Mesh(std::vector<ObjLoader::loadedOBJ> loadedOBJs) {
	auto meshes = std::move(loadedOBJs);
	loadedOBJ = meshes.front();
	coordinates = meshes.front().vertices;
	setTextureCoords(meshes.front().texCoords);
	setNormals(meshes.front().normals);
	vao = new VertexArray;
	material = loadedOBJ.material;
	for (int i = 1; i < meshes.size(); ++i) {
	  relatedMeshes.emplace_back(meshes[i]);
	  relatedMeshes.back().compile();
	}
	model = glm::mat4(1.f);
	setTextures(material.textures);
  }

  explicit Mesh(const ObjLoader::loadedOBJ &loadedObjData) {
	coordinates = loadedObjData.vertices;
	setTextureCoords(loadedObjData.texCoords);
	setNormals(loadedObjData.normals);
	vao = new VertexArray;
	model = glm::mat4(1.f);
	material = loadedOBJ.material;
	setTextures(material.textures);
  }

  ObjLoader::loadedOBJ loadedOBJ;
  ObjLoader::MaterialInfo material;

  Mesh *compile() {
	if (coordinates.empty()) {
	  LOG_S(ERROR) << "Coordinates were not set!";
	  return this;
	}
	addNewBuffer(VertexBuffer(coordinates));// Setting VBO
	if (textures.size() == 1) {
	  addTexture("textures/NoSpec.png");
	}
	fillVAO();
	return this;
  }

  Mesh *setColor(glm::vec3 color) {
	std::vector<float> colors;
	for (auto &coord : coordinates) {
	  colors.push_back(color.r);
	  colors.push_back(color.g);
	  colors.push_back(color.b);
	}
	addNewBuffer(ColorBuffer(colors), true);
	return this;
  }

  Mesh *addTexture(std::string filePath) {
	textures.push_back(new Texture(std::move(filePath)));
	if (!wasBufferDefined(Buffer::TEXTURE_COORDS)) {
	  LOG_S(INFO) << "Generating textureCoords";
	  auto texCoords = Texture::generateTextureCoords(coordinates.size() / 3);
	  addNewBuffer(TextureBuffer(texCoords));
	}
	for (auto &mesh : relatedMeshes) {
	  mesh.setTextures(textures);
	}
	return this;
  }
  Mesh *addScaledTexture(std::string filePath, glm::vec2 texScale) {
	textures.push_back(new Texture(std::move(filePath)));
	LOG_S(INFO) << "Generating textureCoords";
	auto texCoords = Texture::generateTextureCoords(coordinates.size() / 3, {texScale.x, texScale.y});
	addNewBuffer(TextureBuffer(texCoords), true);
	for (auto &mesh : relatedMeshes) {
	  mesh.setTextures(textures);
	}
	return this;
  }

  Mesh *setNormals(std::vector<glm::vec3> normals) {
	addNewBuffer(NormalsBuffer(std::move(normals)), true);
	return this;
  }

  void generateNormals() {
	auto vertices = floatArrayToVec3Array(coordinates);
	std::vector<glm::vec3> normals{};
	for (int i = 0; i < vertices.size(); ++i) {
	  normals.push_back(calculateNormal(vertices[i], vertices[i + 1], vertices[i + 2]));
	}
	// normals.push_back(glm::triangleNormal(vertices[vertices.size()-1], vertices[0], vertices[1]));

	setNormals(normals);
  }

  static glm::vec3 calculateNormal(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
	glm::vec3 x = {b.x - a.x, b.y - a.y, b.z - a.z};
	glm::vec3 y = {c.x - a.x, c.y - a.y, c.z - a.z};

	auto cross = glm::normalize(glm::cross(x, y));
	return cross;
  }

  Mesh *setNormals(std::vector<float> normals) {
	addNewBuffer(NormalsBuffer(std::move(normals)), true);
	return this;
  }

  Mesh *setTextureCoords(std::vector<float> textureCoords) {
	addNewBuffer(TextureBuffer(std::move(textureCoords)), true);
	return this;
  }

  void setIndices(std::vector<unsigned int> indices) {
	indexBufferSize = indices.size();
	indexBuffer = new IndexBuffer(indices);
  }

 private:
  Mesh *addNewBuffer(Buffer _buffer, bool bReplace = false) {
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

  Mesh *fillVAO() {
	if (!wasBufferDefined(Buffer::NORMAL)) {
	  generateNormals();
	}
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

  Buffer *getBufferOfType(Buffer::type bufferType) {
	for (auto &buffer : buffers) {
	  if (bufferType == buffer.bufferType) {
		return &buffer;
	  }
	}
	return nullptr;
  }

 public:
  Mesh *setScale(glm::vec3 _scale) {
	scale = _scale;

	updateModel();
	for (auto &mesh : relatedMeshes) {
	  mesh.setScale(scale);
	}
	return this;
  }

  Mesh *setRotation(glm::vec3 _rotation) {
	rotation = _rotation;
	updateModel();
	for (auto &mesh : relatedMeshes) {
	  mesh.setRotation(rotation);
	}
	return this;
  }

  Mesh *setPosition(glm::vec3 _position) {
	position = _position;
	updateModel();
	for (auto &mesh : relatedMeshes) {
	  mesh.setPosition(position);
	}
	return this;
  }

  Mesh *setOrigin(glm::vec3 _origin) {
	origin = _origin;
	updateModel();
	for (auto &mesh : relatedMeshes) {
	  mesh.setOrigin(origin);
	}
	return this;
  }

  Mesh *updateModel() {
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

  Mesh *setTextures(std::vector<Texture *> _textures) {
	textures = std::move(_textures);
	return this;
  }
};

#endif//CGLABS__MESH_HPP_
