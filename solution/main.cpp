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
#include "fileHandler_NEW.h"
#include "hashtable.h"
#include "licenseChecker.h"
/*
void movementKey(vec3 moveAmount) {
    vec3 camPos = currentScene->currentCamera.getPosition();
    vec3 newPos = moveAmount + camPos;
    currentScene->currentCamera.move(newPos);
}
*/

/*
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
*/

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

static void objRegex(std::string line, std::regex reg, std::vector<float>* vec, int len) {
    std::smatch matchObj;
    std::regex_match(line, matchObj, reg);

    if (matchObj.size() < 2) {
        throw "something went wrong";
    }

    for (int i = 0; i < len; ++i) {
        vec->push_back(std::stof(matchObj[i + 1].str()));
    }
}

static unsigned int compileShader(std::string filepath, GLenum type) {
    std::ifstream text(filepath);
    if (!text.is_open()) {
        std::cout << filepath << "\ndoesn't exist\n";
        return 0;
    }

    std::stringstream buffer;
    buffer << text.rdbuf();
    std::string temp = buffer.str();
    const char* shaderText = temp.c_str();	//extract c string version of the file text
    std::cout << shaderText << "\n";
    text.close();

    unsigned int shaderpart = glCreateShader(type);
    glShaderSource(shaderpart, 1, &shaderText, NULL);	//send shaderpart text to opengl for compilation
    glCompileShader(shaderpart);	//compile shader

    int result;
    glGetShaderiv(shaderpart, GL_COMPILE_STATUS, &result);	//check if shader compiled properly
    if (!result) {	//output error and cleanup if it didnt
        char errLog[512];
        glGetShaderInfoLog(shaderpart, 512, NULL, errLog);
        std::cout << errLog << "\n";
        glDeleteShader(shaderpart);
        return 0;
    }

    return shaderpart;
};

std::vector<float> readOBJ_old_2(std::string filepath) {
    std::vector<float> verticies;
    std::vector<float> vertexPos;
    std::vector<float> vertexNor;
    std::vector<float> vertexTex;

    std::ifstream obj(filepath);
    if (!obj.is_open()) {
        return verticies;
    }

    static const std::regex v("v (-?[0-9]+.[0-9]*) (-?[0-9]+.[0-9]*) (-?[0-9]+.[0-9]*)");
    static const std::regex vn("vn (-?[0-9]+.[0-9]*) (-?[0-9]+.[0-9]*) (-?[0-9]+.[0-9]*)");
    static const std::regex vt("vt (-?[0-9]+.[0-9]*) (-?[0-9]+.[0-9]*)");
    static const std::regex f("f ([0-9]*/[0-9]*/[0-9]*) ([0-9]*/[0-9]*/[0-9]*) ([0-9]*/[0-9]*/[0-9]*)");
    static const std::regex index("([0-9]*)/([0-9]*)/([0-9]*)");

    for (std::string line; std::getline(obj, line); ) {
        switch (line.c_str()[0] << sizeof(char) * 8 | line.c_str()[1]) {
        case * "v" << sizeof(char) * 8 | *" ": // 30240
            objRegex(line, v, &vertexPos, 3);

            break;

        case * "v" << sizeof(char) * 8 | *"n": // 30318
            objRegex(line, vn, &vertexNor, 3);

            break;

        case * "v" << sizeof(char) * 8 | *"t": // 30324
            objRegex(line, vt, &vertexTex, 2);


            break;
        case * "f" << sizeof(char) * 8 | *" ": // 26144
            std::smatch matchObj;
            std::regex_match(line, matchObj, f);

            if (matchObj.size() < 2) {
                throw "malformed obj file"; // malformed obj file
            }

            std::vector<std::string> str_f;
            for (int i = 0; i < 3; i++) {
                str_f.push_back(matchObj[i + 1].str());
            }

            for (int i = 0; i < 3; i++) {
                std::smatch faceIndices;
                std::regex_match(str_f[i], faceIndices, index);

                verticies.push_back(vertexPos[(std::stoi(faceIndices[1].str()) - 1) * 3]);
                verticies.push_back(vertexPos[(std::stoi(faceIndices[1].str()) - 1) * 3 + 1]);
                verticies.push_back(vertexPos[(std::stoi(faceIndices[1].str()) - 1) * 3 + 2]);

                verticies.push_back(vertexTex[(std::stoi(faceIndices[2].str()) - 1) * 2]);
                verticies.push_back(vertexTex[(std::stoi(faceIndices[2].str()) - 1) * 2 + 1]);

                verticies.push_back(vertexNor[(std::stoi(faceIndices[3].str()) - 1) * 3]);
                verticies.push_back(vertexNor[(std::stoi(faceIndices[3].str()) - 1) * 3 + 1]);
                verticies.push_back(vertexNor[(std::stoi(faceIndices[3].str()) - 1) * 3 + 2]);
            }

            break;
        }
    }

    return verticies;
}

int main()
{
    std::cout << checkLicense("IJIF-TJCW-GTOA-MMZI", "mrbfdi@outlook.com") << "\n\n";

    GLFWwindow* window = initGL();

    std::vector<float> vertices = readOBJ_old_2("meshes/monkey.obj");

    unsigned int newvao;
    unsigned int newvbo;

    glGenVertexArrays(1, &newvao);
    glBindVertexArray(newvao);

    glGenBuffers(1, &newvbo);
    glBindBuffer(GL_ARRAY_BUFFER, newvbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0); // positions
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // textures
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float))); // normals
    glEnableVertexAttribArray(2);

    unsigned int vert = compileShader("shaders/vertexShader.vert", GL_VERTEX_SHADER);
    unsigned int frag = compileShader("shaders/fragmentShader.frag", GL_FRAGMENT_SHADER);

    unsigned int shaderID = glCreateProgram();
    glAttachShader(shaderID, vert);
    glAttachShader(shaderID, frag);

    glUseProgram(shaderID);
    /*
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
    std::cout << monkey.vao << "\n" << monkey.vbo << "\n" << monkey.size;
    mesh square = newFileloader.meshLoader("meshes/bettercube.obj");
    texture box = newFileloader.imageLoader("textures/box.png");
    texture checkers = newFileloader.imageLoader("textures/checkermap_resized.png");

    shader newShader = newFileloader.shaderLoader({ "shaders/fragmentShader.frag", "shaders/vertexShader.vert" });

    camera newCam;
    newCam.changePerspective(toRad(70.0f), 1.0f, 1.0f, 30.0f);
    newCam.move({ 0.0f, 0.0f, 0.0f });

    scene myscene(window);
    myscene.currentCamera = newCam;

    instance instance1("instance1", &myscene, &monkey, &box, &newShader);
    instance1.drawImplementation = &drawImplementation;
    instance1.move({ 0.0f, 0.0f, -6.5f });
    */
    
    while (!glfwWindowShouldClose(window)) {
        unsigned int queries[2];
        glGenQueries(2, queries);
        glQueryCounter(queries[0], GL_TIMESTAMP);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f, 1.0f);

        static const int lightPos = glGetUniformLocation(shaderID, "lightPosition");
        static const int trans = glGetUniformLocation(shaderID, "trans");
        static const int cam = glGetUniformLocation(shaderID, "cam");
        static const int proj = glGetUniformLocation(shaderID, "proj");
        mat4 transformation = graphics::createTranslation( 0.0f, 0.0f, -6.5f );
        mat4 camera = iden4();
        mat4 perspective = graphics::createPerspective(toRad(70.0f), 1.0f, 1.0f, 30.0f);

        float lightPosition[3] = { 5.0f, 5.0f, 5.0f };
        glUniform3fv(lightPos, 1, lightPosition);
        glUniformMatrix4fv(trans, 1, true, transformation.array);
        glUniformMatrix4fv(cam, 1, true, camera.array);
        glUniformMatrix4fv(proj, 1, true, perspective.array);

        glDrawArrays(GL_TRIANGLES, 0, vertices.size());

        
        //instance1.draw();

        //instance1.rotate({ 0.0f, static_cast<float>(glfwGetTime()), 0.0f });

        //newScene.currentCamera.move({ static_cast<float>(sin(glfwGetTime())), 0.0f, 0.0f });

        //instance2.draw();
        //instance2.rotate({ 0.0f, static_cast<float>(glfwGetTime()), 0.0f });

        glfwSwapBuffers(window);
        glfwPollEvents();

        glQueryCounter(queries[1], GL_TIMESTAMP);

        unsigned long long start, stop;
        glGetQueryObjectui64v(queries[0], GL_QUERY_RESULT, &start);
        glGetQueryObjectui64v(queries[1], GL_QUERY_RESULT, &stop);

        //std::cout << (stop - start) / 1000000 << "\n";
    }

    glfwTerminate();
 
    return 0;
}

