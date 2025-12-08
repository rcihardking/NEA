#define _CRT_SECURE_NO_WARNINGS

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <iomanip>

#include "fileLoader.h"
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

    /*
    std::vector<std::string> images;
    images.push_back("../textures/box.png");
    images.push_back("../textures/checkermap_resized.png");
    imageLoader imageLoader;

    imageLoader.addImages(images);
    std::vector<GLuint> textures = imageLoader.genImages();

    meshLoader meshLoader;
    */

    /*
    image newImage("../textures/box.png");
    newImage.createTexture();

    image newImage1("../textures/checkermap_resized.png");
    newImage1.createTexture();
    */

    /*
    graphics::shader newShader = { "../shaders/vertexShader.txt", "../shaders/fragmentShader.txt" };

    graphics::mesh mesh1(
        "../meshes/monkey.obj",
        newShader.ID,
        textures[0]
    );
    mesh1.move({ 1.5f, 0.0f, -6.5f });
   
    graphics::mesh mesh2(
        "../meshes/cube.obj",
        newShader.ID, 
        textures[1]
    );
    mesh2.move({ -1.5f, 0.0f, -6.5f });
    */

    //graphics::staticShader newShader = { "../shaders/vertexShader.txt", "../shaders/fragmentShader.txt" };
    oldgraphics::shader oldShader = { "../shaders/vertexShader.txt", "../shaders/fragmentShader.txt" };

    //graphics::scene newScene;
    //int planeIndex = newScene.loadMesh("../meshes/cube.obj");
    //int monkeyIndex = newScene.loadMesh("../meshes/monkey.obj", newvao);

    //int boxIndex = newScene.loadImage("../textures/box.png");

    //graphics::staticInstance mesh1 = { &newScene, &newShader, planeIndex, boxIndex };
    //mesh1.move({ -6.5f, 0.0f, -1.5f });
    //graphics::staticInstance mesh2 = { &newScene, &newShader, monkeyIndex, boxIndex };
    //mesh2.move({ -6.5f, 0.0f, 1.5f });

    oldgraphics::mesh oldmesh("../meshes/cube.obj", oldShader.ID, 0);
    oldmesh.move({ -6.5f, 0.0f, 1.5f });


    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(48.0f / 255.0f, 213.0f / 255.0f, 200.0f / 255.0f, 1.0f);

        /*
        mesh1.draw();
        mesh1.rotate({ 0.0f, static_cast<float>(glfwGetTime()), 0.0f });
        mesh2.draw();
        mesh2.rotate({ static_cast<float>(glfwGetTime()), 0.0f, 0.0f });
        */

        //mesh1.draw();
        //mesh2.draw();
        oldmesh.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
