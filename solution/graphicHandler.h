#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "graphicMath.h"
#include "fileHandler.h"

#include <glad/glad.h>
#include <libpng/png.h>
#include <glfw/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>
#include <vector>
#include <array>
#include <regex>
#include <map>

class location {
protected:
    float position[3] = { 0.0f, 0.0f, 0.0f };
    float orientation[3] = { 0.0f, 0.0f, 0.0f };
    float size = 1.0f;

    mat4 translation = iden4();
    mat4 rotation = iden4();
    mat4 scale = iden4();
public:
    inline vec3 getPosition() {
        vec3 vector = position;
        return vector;
    };
    virtual void move(std::initializer_list<float> pos);
    virtual void move(vec3 pos);
    inline vec3 getOrientation() {
        vec3 vector = orientation;
        return vector;
    };
    virtual void rotate(std::initializer_list<float> rot);
    virtual void rotate(vec3 rot);
    inline float getSize() {
        return size;
    };
    virtual void resize(float factor);

    mat4 getTransformation();
};

class camera : public location {
public:
    void rotate(std::initializer_list<float> rot) override;
    void rotate(vec3 rot) override;
    void move(std::initializer_list<float> pos) override;
    void move(vec3 rot) override;
    void resize(float factor) override;

    inline mat4 getPerspective() {
        return perspective;
    }
    void changePerspective(float fov, float aspect, float n, float f);
private:
    mat4 perspective;
};

class light : public location {
public:
    float intensity = 1.0f;
};

class scene {
public:
    scene(GLFWwindow* renderingWindow = nullptr) : window{ renderingWindow } {};
    inline void changeWindow(GLFWwindow* newWindow) {
        window = newWindow;
    };

    camera currentCamera;
    std::vector<light> lights;
    float ambientConstant = 0.6f;
private:
    GLFWwindow* window;
};

class instance : public location {
private:
    std::string identifier;
    scene* myScene;
    mesh* myMesh;
    texture* myTexture;
    shader* myShader;

    float shinyness;
    float reflectivity;

protected:
    instance* parent = nullptr;
    std::vector<instance*> children;

public:
    inline instance(std::string name, scene* scene, mesh* model, texture* img, shader* shad, float shiny = 2.0f, float reflect = 1.0f) {
        identifier = name;
        myScene = scene;
        myMesh = model;
        myTexture = img;
        myShader = shad;
        shinyness = shiny;
        reflectivity = reflect;
    };

    instance* search(std::string name);

    void draw();
    void (*drawImplementation)(instance*, scene*, mesh*, texture*, shader*, float, float) = nullptr;

    inline const std::vector<instance*> getChildren() { return static_cast<const std::vector<instance*>>(children); };
    inline instance* getParent() { return parent; };

    void changeParent(instance* newParent);

    void rotate(std::initializer_list<float> rot) override;
    void rotate(vec3 rot) override;
    void move(std::initializer_list<float> pos) override;
    void move(vec3 pos) override;
    void resize(float factor) override;
};

void drawImplementation(instance* self, scene* myScene, mesh* myMesh, texture* myTexture, shader* myShader, float shinyness, float reflectivity);