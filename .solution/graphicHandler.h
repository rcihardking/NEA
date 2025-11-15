#pragma once

#include "graphicMath.h"

#include <glad/glad.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>
#include <vector>
#include <array>
#include <regex>



namespace graphics {
    class shader {
    public:
        GLuint ID;

        shader(const char* vertFilepath, const char* fragFilepath);
    private:
        std::vector<GLuint> shaders;

        GLuint compileShader(const char* filepath, GLenum type);
        void cleanupShaders();
    };

    class mesh {
    public:
        std::array<float, 3> position;
        std::array<float, 3> orientation;
        float factor;

        mesh(GLuint shaderID, const char* filepath, GLuint textureID, float posX, float posY, float posZ, float rotX, float rotY, float rotZ, float scale);
        void remove();
        void draw();
    private:
        std::vector<float> verticies;

        GLuint texture = 0;

        int polySize = 3;

        GLuint vbo;
        
        GLuint vao;
        GLuint ebo;

        GLuint shader;

        void parseObj(const char* filepath);
    };
}
