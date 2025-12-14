#define _CRT_SECURE_NO_WARNINGS

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <iomanip>

#include "imageHandler.h"
#include "graphicHandler.h"
#include "graphicMath.h"

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_E:
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

std::string getTime()
{
    // time function taken from stack overflow

    using namespace std::chrono;

    // get current time
    auto now = system_clock::now();

    // get number of milliseconds for the current second
    // (remainder after division into seconds)
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    // convert to std::time_t in order to convert to std::tm (broken time)
    auto timer = system_clock::to_time_t(now);

    // convert to broken time
    std::tm bt = *std::localtime(&timer);

    std::ostringstream oss;

    oss << std::put_time(&bt, "%H:%M:%S"); // HH:MM:SS
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return oss.str();
}

void error_callback(int error, const char* desc) {
    std::cout << error << " " << desc << std::endl;
}

int main()
{
    if (!glfwInit()) {
        return -1;
    }

    glfwSetErrorCallback(error_callback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 800, "Test Window", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGL()) {
        return -1;
    }
    glfwSetKeyCallback(window, keyCallback);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    newgraphics::staticShader newerShader = { "../shaders/vertexShader.txt", "../shaders/fragmentShader.txt" };
    newgraphics::scene newScene;

    int cube = newScene.loadMesh("../meshes/cube.obj", 0);
    int monkey = newScene.loadMesh("../meshes/monkey.obj", 0);
    int box = newScene.loadImage("../textures/box.png");

    newgraphics::staticInstance instance1 = { &newScene, &newerShader, cube, box };
    instance1.move({ -1.5f, 0.0f, -6.5f });

    newgraphics::staticInstance instance2 = { &newScene, &newerShader, monkey, box };
    instance2.move({ 1.5f, 0.0f, -6.5f });

    instance2.changeParent(&instance1);
    

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(48.0f / 255.0f, 213.0f / 255.0f, 200.0f / 255.0f, 1.0f);

        instance1.draw();
        instance1.rotate({ 0.0f, static_cast<float>(glfwGetTime()), 0.0f });

        //instance2.draw();
        //instance2.rotate({ 0.0f, static_cast<float>(glfwGetTime()), 0.0f });

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
