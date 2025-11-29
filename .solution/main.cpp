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
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

   // image newImage("../textures/checkermap_resized.png");
    image newImage("../textures/box.png");
    newImage.createTexture();

    graphics::shader newShader = { "../shaders/vertexShader.txt", "../shaders/fragmentShader.txt" };

    graphics::mesh mesh1(
        newShader.ID,
        "../meshes/monkey.obj",
        newImage.texture,
        { 1.5f, 0.0f, -6.5f },
        { 0.0f, 0.0f, 0.0f },
        1.0f
    );
    graphics::mesh mesh2(
        newShader.ID,
        "../meshes/plane.obj",
        newImage.texture,
        { -1.5f, 0.0f, -6.5f },
        { toRad(90.0f), 0.0f, 0.0f },
        1.0f
    );

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(48.0f / 255.0f, 213.0f / 255.0f, 200.0f / 255.0f, 1.0f);

        mesh1.draw();
        mesh1.rotate({ 0.0f, static_cast<float>(glfwGetTime()), 0.0f });
        mesh2.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
}
    return 0;
}
