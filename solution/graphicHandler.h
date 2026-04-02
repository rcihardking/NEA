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

//superclass that has the required members and functions for location data
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

class camera : public location {    //class used to represent the cameras location also stores perspective
public:
    //overrides default location functions to reverse input values
    //this makes it act like a camera
    void rotate(std::initializer_list<float> rot) override;
    void rotate(vec3 rot) override;
    void move(std::initializer_list<float> pos) override;
    void move(vec3 rot) override;
    void resize(float factor) override;

    inline mat4 getPerspective() {  
        return perspective;
    }
    void changePerspective(float fov, float aspect, float n, float f);  //replaces perspective matrix if we want to change it
private:
    mat4 perspective;
};

class light : public location { //stores a lights data
public:
    float intensity = 1.0f; //the intensity of the light
};

static void error_callback(int error, const char* desc) {
    std::cout << error << " " << desc << std::endl;
}

class scene {   //creates the window and stores the lights, camera and ambient constant 
public:
    scene();

    camera currentCamera;
    std::vector<light> lights;
    float ambientConstant = 0.6f;
    GLFWwindow* window;
};

class instance : public location {  //stores all the relevant mesh data so things can be directly rendered from it
private:
    std::string identifier; //name of the instance so it can be searched for

    //model data
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

    //drawing functions
    void draw();
    void (*drawImplementation)(instance*, scene*, mesh*, texture*, shader*, float, float) = nullptr;    //lets us give different draw implementations to instances

    //child / parent tree functions
    inline const std::vector<instance*> getChildren() { return static_cast<const std::vector<instance*>>(children); };
    inline instance* getParent() { return parent; };

    void changeParent(instance* newParent);

    //movement functions
    void rotate(std::initializer_list<float> rot) override;
    void rotate(vec3 rot) override;
    void move(std::initializer_list<float> pos) override;
    void move(vec3 pos) override;
    void resize(float factor) override;
};

void drawImplementation(instance* self, scene* myScene, mesh* myMesh, texture* myTexture, shader* myShader, float shinyness, float reflectivity);   //default draw implementation