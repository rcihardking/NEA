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
#include "maths.h"

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

    graphicsmath::matrix<2, 2> a = { 3, 2, 2, 2 };
    graphicsmath::matrix<2, 2> b = { 4, 3, 3, 3 };

    b*a;

    glfwSetErrorCallback(error_callback);
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

    GLuint texture = createTexture("../textures/box.png"); // for some reason this causes errors with the new texture!
    graphics::shader newShader = { "../shaders/vertexShader.txt", "../shaders/fragmentShader.txt" };

    graphics::mesh mesh1(newShader.ID, 
        "../meshes/monkey.obj", 
        texture, 
        graphicmath::vec3({  1.5f, 0.0f, -2.5f }), 
        graphicmath::vec3({ 0.0f, 0.0f, 0.0f}),
        1.0f
    );
    graphics::mesh mesh2(newShader.ID, 
        "../meshes/monkey.obj", 
        texture, 
        graphicmath::vec3({ -1.5f, 0.0f, -2.5f }), 
        graphicmath::vec3({ 0.0f, 0.0f, 0.0f}),
        1.0f
    );

    GLuint loc1 = glGetUniformLocation(newShader.ID, "rotation");

    while (!glfwWindowShouldClose(window)) {
        glClearColor(48.0f / 255.0f, 213.0f / 255.0f, 200.0f / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        mesh1.orientation = graphicmath::vec3({0.0f, (float)glfwGetTime(), 0.0f});
        mesh1.draw();
        mesh2.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteTextures(1, &texture);
    mesh1.remove();
    mesh2.remove();
    glDeleteProgram(newShader.ID);

    glfwTerminate();

    return 0;
}
