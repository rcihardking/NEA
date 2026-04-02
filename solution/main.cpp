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
#include "fileHandler.h"
#include "hashtable.h"
#include "licenseChecker.h"

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
    //glfwSetKeyCallback(window, keyCallback);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    return window;
}

int main()
{
    std::cout << checkLicense("IJIF-TJCW-GTOA-MMZI", "mrbfdi@outlook.com") << "\n";

    GLFWwindow* window = initGL();

    hashtable<readImgPtr, 10> default_imageFormatLoaders = {
        {"png", &readPNG}
    };
    hashtable<readMeshPtr, 10> default_meshFormatLoaders = {
        {"obj", &readOBJ_old}
    };
    
    formatLoaders formats;
    formats.imgFormats = &default_imageFormatLoaders;
    formats.meshFormats = &default_meshFormatLoaders;

    fileLoader newFileloader(formats);
    mesh monkey = newFileloader.meshLoader("meshes/monkey.obj");
    mesh square = newFileloader.meshLoader("meshes/bettercube.obj");
    texture box = newFileloader.imageLoader("textures/box.png");
    texture checkers = newFileloader.imageLoader("textures/checkermap_resized.png");

    shader newShader = newFileloader.shaderLoader({ "shaders/fragmentShader.frag", "shaders/vertexShader.vert" });

    camera newCam;
    newCam.changePerspective(toRad(70.0f), 1.0f, 1.0f, 30.0f);

    light newLight;
    newLight.intensity = 10.0f;
    newLight.move({ 5.0f, 5.0f, 5.0f });

    scene newscene(window);
    newscene.currentCamera = newCam;
    newscene.lights.push_back(newLight);

    instance instance1("instance1", &newscene, &monkey, &box, &newShader);
    instance1.drawImplementation = &drawImplementation;
    instance1.move({ 0.0f, 0.0f, -6.5f });
    
    while (!glfwWindowShouldClose(window)) {
        //unsigned int queries[2];
        //glGenQueries(2, queries);
        //glQueryCounter(queries[0], GL_TIMESTAMP);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1.0f);

        
        instance1.draw();

        instance1.rotate({ 0.0f, static_cast<float>(glfwGetTime()), 0.0f /*static_cast<float>(glfwGetTime())*/});

        //newscene.currentCamera.move({ static_cast<float>(sin(glfwGetTime())), 0.0f, 0.0f });

        //instance2.draw();
        //instance2.rotate({ 0.0f, static_cast<float>(glfwGetTime()), 0.0f });

        glfwSwapBuffers(window);
        glfwPollEvents();

        //glQueryCounter(queries[1], GL_TIMESTAMP);

        //unsigned long long start, stop;
        //glGetQueryObjectui64v(queries[0], GL_QUERY_RESULT, &start);
        //glGetQueryObjectui64v(queries[1], GL_QUERY_RESULT, &stop);

        //std::cout << (stop - start) / 1000000 << "\n";
    }

    glfwTerminate();
 
    return 0;
}

