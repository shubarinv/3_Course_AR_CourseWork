
#ifndef CGLABS__TEXTURE_HPP_
#define CGLABS__TEXTURE_HPP_

#include <utility>
#include "functions.hpp"

class Texture {
private:
    unsigned int rendererID{};
    std::string filepath{};
    std::vector<unsigned char> localBuffer{};
    int width, height, nrChannels;

public:
    explicit Texture(std::string _filepath) {
        glGenTextures(1, &rendererID);
        glBindTexture(GL_TEXTURE_2D, rendererID);
// set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// load and generate the texture
        stbi_set_flip_vertically_on_load(1);
        unsigned char *data = stbi_load(_filepath.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            if (nrChannels == 3) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            } else {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            }
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            LOG_S(WARNING) << "Failed to load texture at "+filepath;
			std::cout<<filepath<<std::endl;
        }
        stbi_image_free(data);
        unbind();
    }

    ~Texture() {
        glCall(glDeleteTextures(1, &rendererID));
    }

    void bind(unsigned int slot = 0) const {
        glCall(glActiveTexture(GL_TEXTURE0 + slot));
        glCall(glBindTexture(GL_TEXTURE_2D, rendererID));
    }

    [[maybe_unused]] static void unbind() {
        glCall(glBindTexture(GL_TEXTURE_2D, 0));
    }

    [[nodiscard]] unsigned int getWidth() const { return width; }

    [[nodiscard]] unsigned int getHeight() const { return height; }


    static std::vector<float> generateTextureCoords(unsigned int size, glm::vec2 scale = {1, 1}) {
        float texCoordsPreset[] = {
                0.0f, 0.0f,
                1.0f, 0.0f,
                1.0f, 1.0f,

                1.0f, 1.0f,
                0.0f, 1.0f,
                0.0f, 0.0f};
        std::vector<float> textureCoords;
        short presetNum{0};
        for (int i = 0; i < size; i++) {
            if (presetNum > 11)presetNum = 0;
            textureCoords.push_back(texCoordsPreset[presetNum] * scale.x);
            textureCoords.push_back(texCoordsPreset[presetNum + 1] * scale.y);
            presetNum += 2;
        }

        return textureCoords;
    }

    [[nodiscard]] GLuint getID() const {
        return rendererID;
    }
};

#endif //CGLABS__TEXTURE_HPP_