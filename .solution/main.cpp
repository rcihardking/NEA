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

    graphics::mesh mesh1("../meshes/monkey.obj", graphicmath::vec3({  1.5f, 0.0f, -2.5f }), 1.0f);
    graphics::mesh mesh2("../meshes/monkey.obj", graphicmath::vec3({ -1.5f, 0.0f, -2.5f }), 1.0f);

    //graphicmath::matrix rotation;

    graphics::shader newShader = { "../shaders/vertexShader.txt", "../shaders/fragmentShader.txt" };

    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);

    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);

    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    //glEnableVertexAttribArray(2);

    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindVertexArray(0);

    GLuint loc1 = glGetUniformLocation(newShader.ID, "rotation");

    while (!glfwWindowShouldClose(window)) {
        glClearColor(48.0f / 255.0f, 213.0f / 255.0f, 200.0f / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //glUseProgram(newShader.ID);
        //rotation = graphicmath::matRotation(graphicmath::vec3({ 1.0f, 0.0f, 0.0f }), 2.0f * static_cast<float>( glfwGetTime() ));
        //glUniformMatrix4fv(loc1, 1, true, rotation.array.data());

        mesh1.draw(newShader);
        mesh2.draw(newShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //glDeleteTextures(1, &texture);
    mesh1.remove();
    mesh2.remove();
    glDeleteProgram(newShader.ID);

    glfwTerminate();

    return 0;
}
