
#ifndef CGLABS__LIGHTS_MANAGER_HPP_
#define CGLABS__LIGHTS_MANAGER_HPP_

#include <utility>
#include <variant>
#include <vector>
#include "shader.hpp"

class LightsManager {
public:
    struct DirectionalLight {
        DirectionalLight(std::string _name, glm::vec3 _direction, glm::vec3 _ambient, glm::vec3 _diffuse,
                         glm::vec3 _specular) {
            name = std::move(_name);
            direction = _direction;
            ambient = _ambient;
            diffuse = _diffuse;
            specular = _specular;
        }

        std::string name;
        glm::vec3 direction{}; // this is where light will look
        glm::vec3 ambient{};

        glm::vec3 diffuse{}; //colors ?
        glm::vec3 specular{}; //colors ?
    };

    struct PointLight {
	  /**
	   *
	   * @param _name ммя
	   * @param _position местоположение
	   * @param _ambient глобальный цвет
	   * @param _diffuse цвет
	   * @param _specular цвет блика
	   * @param _constant что-то
	   * @param _linear что-то
	   * @param _quadratic что-то
	   */
        PointLight(std::string _name,
                   glm::vec3 _position,

                   glm::vec3 _ambient,
                   glm::vec3 _diffuse,
                   glm::vec3 _specular,

                   float _constant,
                   float _linear,
                   float _quadratic) {
            name = std::move(_name);
            position = _position;
            ambient = _ambient;
            diffuse = _diffuse;
            specular = _specular;
            constant = _constant;
            linear = _linear;
            quadratic = _quadratic;
        }

        std::string name;
        glm::vec3 position{};

        glm::vec3 ambient{};
        glm::vec3 diffuse{};
        glm::vec3 specular{};

        float constant;
        float linear;
        float quadratic;
    };

    struct SpotLight {
	  /**
	   *
	   * @param _name имя? звание?
	   * @param _position
	   * @param _direction
	   * @param _ambient
	   * @param _diffuse
	   * @param _specular
	   * @param _cutoff
	   * @param _outerCutOff
	   * @param _constant
	   * @param _linear
	   * @param _quadratic
	   */
	  SpotLight(std::string _name,
                glm::vec3 _position,
                glm::vec3 _direction,

                glm::vec3 _ambient,
                glm::vec3 _diffuse,
                glm::vec3 _specular,

                float _cutoff,
                float _outerCutOff,
                float _constant,
                float _linear,
                float _quadratic){
        name = std::move(_name);
        position = _position;
        direction = _direction;
        ambient = _ambient;
        diffuse = _diffuse;
        specular = _specular;
        constant = _constant;
        linear = _linear;
        quadratic = _quadratic;
		cutOff=_cutoff;
		outerCutOff=_outerCutOff;
	  }
        std::string name;
        glm::vec3 position{};
        glm::vec3 direction{};

        float cutOff{};
        float outerCutOff{};
        float constant{};
        float linear{};
        float quadratic{};

        glm::vec3 ambient{};
        glm::vec3 diffuse{};
        glm::vec3 specular{};
    };
private:
    std::vector<PointLight> pointLights{};
    std::vector<DirectionalLight> dirLights{};
    std::vector<SpotLight> spotLights{};

public:
    void passDataToShader(Shader *shader) {
        shader->bind();
        shader->setUniform1i("NUM_POINT_LIGHTS", pointLights.size());
        shader->setUniform1i("NUM_SPOT_LIGHTS", spotLights.size());
        shader->setUniform1i("NUM_DIR_LIGHTS", dirLights.size());
        for (int i = 0; i < dirLights.size(); ++i) {
            shader->setUniform3f("dirLights[" + std::to_string(i) + "].direction", dirLights[i].direction);
            shader->setUniform3f("dirLights[" + std::to_string(i) + "].diffuse", dirLights[i].diffuse);
            shader->setUniform3f("dirLights[" + std::to_string(i) + "].ambient", dirLights[i].ambient);
            shader->setUniform3f("dirLights[" + std::to_string(i) + "].specular", dirLights[i].specular);
        }
        for (int i = 0; i < spotLights.size(); ++i) {
            shader->setUniform3f("spotLight[" + std::to_string(i) + "].direction", spotLights[i].direction);
            shader->setUniform3f("spotLight[" + std::to_string(i) + "].position", spotLights[i].position);
            shader->setUniform1f("spotLight[" + std::to_string(i) + "].cutOff", spotLights[i].cutOff);
            shader->setUniform1f("spotLight[" + std::to_string(i) + "].outerCutOff", spotLights[i].outerCutOff);
            shader->setUniform1f("spotLight[" + std::to_string(i) + "].constant", spotLights[i].constant);
            shader->setUniform1f("spotLight[" + std::to_string(i) + "].linear", spotLights[i].linear);
            shader->setUniform1f("spotLight[" + std::to_string(i) + "].quadratic", spotLights[i].quadratic);
            shader->setUniform3f("spotLight[" + std::to_string(i) + "].diffuse", spotLights[i].diffuse);
            shader->setUniform3f("spotLight[" + std::to_string(i) + "].ambient", spotLights[i].ambient);
            shader->setUniform3f("spotLight[" + std::to_string(i) + "].specular", spotLights[i].specular);

        }
        for (int i = 0; i < pointLights.size(); ++i) {

            shader->setUniform3f("pointLights[" + std::to_string(i) + "].position", pointLights[i].position);
            shader->setUniform1f("pointLights[" + std::to_string(i) + "].constant", pointLights[i].constant);
            shader->setUniform1f("pointLights[" + std::to_string(i) + "].linear", pointLights[i].linear);
            shader->setUniform1f("pointLights[" + std::to_string(i) + "].quadratic", pointLights[i].quadratic);
            shader->setUniform3f("pointLights[" + std::to_string(i) + "].diffuse", pointLights[i].diffuse);
            shader->setUniform3f("pointLights[" + std::to_string(i) + "].ambient", pointLights[i].ambient);
            shader->setUniform3f("pointLights[" + std::to_string(i) + "].specular", pointLights[i].specular);

        }
    }

    DirectionalLight *getDirLightByName(const std::string &name) {
        for (auto &dirLight:dirLights) {
            if (dirLight.name == name)return &dirLight;
        }
        return nullptr;
    }

    PointLight *getPointLightByName(std::string name) {
        for (auto &pointLight:pointLights) {
            if (pointLight.name == name)return &pointLight;
        }
        return nullptr;
    }

    SpotLight *getSpotLightByName(std::string name) {
        for (auto &spotLight:spotLights) {
            if (spotLight.name == name)return &spotLight;
        }
        return nullptr;
    }

public:
    void addLight(const PointLight& pointLight) {
        pointLights.push_back(pointLight);
    }

    void addLight(const SpotLight& spotLight) {
        spotLights.push_back(spotLight);
    }

    void addLight(const DirectionalLight& dirLight) {
        dirLights.push_back(dirLight);
    }
};

#endif//CGLABS__LIGHTS_MANAGER_HPP_
