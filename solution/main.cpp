#define _CRT_SECURE_NO_WARNINGS

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <curl/curl.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <iomanip>

#include "graphicHandler.h"
#include "graphicMath.h"
#include "hashtable.h"
#include "licenseChecker.h"

graphics::scene* currentScene = nullptr;

void movementKey(vec3 moveAmount) {
    vec3 camPos = currentScene->currentCamera.getPosition();
    vec3 newPos = moveAmount + camPos;
    currentScene->currentCamera.move(newPos);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_W:
        {
            vec3 moveAmount = { 0.0f, 0.0f, -1.0f };
            movementKey(moveAmount);
        }
            break;
        case GLFW_KEY_S:
        {
            vec3 moveAmount = { 0.0f, 0.0f, 1.0f };
            movementKey(moveAmount);
        }
            break;
        case GLFW_KEY_A:
        {
            vec3 moveAmount = { -1.0f, 0.0f, 0.0f };
            movementKey(moveAmount);
        }
            break;
        case GLFW_KEY_D:
        {
            vec3 moveAmount = { 1.0f, 0.0f, 0.0f };
            movementKey(moveAmount);
        }
            break;
        case GLFW_KEY_E:
        {
            vec3 moveAmount = { 0.0f, 1.0f, 0.0f };
            movementKey(moveAmount);
        }
            break;
        case GLFW_KEY_Q:
        {
            vec3 moveAmount = { 0.0f, -1.0f, 0.0f };
            movementKey(moveAmount);
        }
            break;
        case GLFW_KEY_TAB:
            int polygonMode[2];
            glGetIntegerv(GL_POLYGON_MODE, polygonMode);
            if (polygonMode[1] == GL_LINE) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }

            break;
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, 1);
            break;
        }
    }
}

void error_callback(int error, const char* desc) {
    std::cout << error << " " << desc << std::endl;
}

GLFWwindow* initGL() {
    if (!glfwInit()) {
        return nullptr;
    }

    glfwSetErrorCallback(error_callback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 800, "Test Window", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGL()) {
        glfwTerminate();
        return nullptr;
    }
    glfwSetKeyCallback(window, keyCallback);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    return window;
}

int main()
{
    std::cout << checkLicense("IJIF-TJCW-GTOA-MMZI", "mrbfdi@outlook.com");
    GLFWwindow* window = initGL();

    graphics::shader newShader = { "shaders/vertexShader.txt", "shaders/fragmentShader.txt" };
    graphics::scene newScene;
    currentScene = &newScene;

    int cube = newScene.loadMesh("meshes/bettercube.obj", 0);
    int monkey = newScene.loadMesh("meshes/monkey.obj", 0);
    int box = newScene.loadImage("textures/box.png");

    //meshLoader meshLoader(3);
    //meshLoader.createVao = &basicVaoCreater;

    //graphics::mesh cube = meshLoader.loadMesh("meshes/bettercube.obj");
    //graphics::mesh monkey = meshLoader.loadMesh("meshes/monkey.obj");

    //textureLoader textureLoader(3);
    //textureLoader.createTexture = &basicTextureCreator;

    //graphics::texture box = textureLoader.loadTexture("textures/box.png");

    //currentScene->meshes.push_back(cube);
    //currentScene->meshes.push_back(monkey);

    //currentScene->textures.push_back(box);


    graphics::instance instance1 = { "cube", &newScene, &newShader, cube, box};

    vec3 test = { -1.5f, 0.0f, -6.5f };
    instance1.move(test);
    instance1.drawImplementation = &drawImplementation;

    graphics::instance instance2 = { "monkey", &newScene, &newShader, monkey, box};
    instance2.changeParent(&instance1);
    instance2.move({ 1.5f, 0.0f, -6.5f });

    
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1.0f);

        instance1.draw();
        instance1.rotate({ 0.0f, static_cast<float>(glfwGetTime()), 0.0f });

        //newScene.currentCamera.move({ static_cast<float>(sin(glfwGetTime())), 0.0f, 0.0f });

        //instance2.draw();
        //instance2.rotate({ 0.0f, static_cast<float>(glfwGetTime()), 0.0f });

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
