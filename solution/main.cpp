#define _CRT_SECURE_NO_WARNINGS

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <curl/curl.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "graphicHandler.h"
#include "graphicMath.h"
#include "fileHandler.h"
#include "hashtable.h"
#include "licenseChecker.h"

int main()
{
    //licenseProcess();   //license key checking process

    scene newscene;

    //create hashmap pointing to read functions
    hashtable<readImgPtr, 10> default_imageFormatLoaders = {
        {"png", &readPNG}
    };
    hashtable<readMeshPtr, 10> default_meshFormatLoaders = {
        {"obj", &readOBJ_old}
    };
    
    formatLoaders formats; 
    formats.imgFormats = &default_imageFormatLoaders;
    formats.meshFormats = &default_meshFormatLoaders;

    //create format loader object
    fileLoader newFileloader(formats);

    //load meshes and textures
    mesh monkey = newFileloader.meshLoader("meshes/monkey.obj");
    mesh square = newFileloader.meshLoader("meshes/bettercube.obj");
    texture box = newFileloader.imageLoader("textures/box.png");
    texture checkers = newFileloader.imageLoader("textures/checkermap_resized.png");

    //load in shader
    shader newShader = newFileloader.shaderLoader({ "shaders/fragmentShader.frag", "shaders/vertexShader.vert" });

    //create necessary objects for rendering
    camera newCam;
    newCam.changePerspective(toRad(70.0f), 1.0f, 1.0f, 30.0f);

    light newLight;
    newLight.intensity = 10.0f;
    newLight.move({ 5.0f, 5.0f, 5.0f });

    newscene.currentCamera = newCam;
    newscene.lights.push_back(newLight);

    instance instance1("instance1", &newscene, &monkey, &box, &newShader);
    instance1.drawImplementation = &drawImplementation;
    instance1.move({ 0.0f, 0.0f, -6.5f });
    
    while (!glfwWindowShouldClose(newscene.window)) {    //render loop
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1.0f);  //set background colour to white

        
        instance1.draw();   //draw instance

        instance1.rotate({ 0.0f, static_cast<float>(glfwGetTime()), 0.0f }); //rotates the object in a circle for the runtime of the application


        glfwSwapBuffers(newscene.window);   //swap to the updated render buffer
    }

    glfwTerminate();
 
    return 0;
}

