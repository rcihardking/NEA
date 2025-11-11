#pragma once

#include "maths.h"

#include <glad/glad.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>
#include <vector>
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
        graphicmath::matrix position;
        graphicmath::matrix orientation;
        float size;

        mesh(GLuint shaderID, const char* filepath, GLuint textureID, graphicmath::matrix pos, graphicmath::matrix rotation, float scale);
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