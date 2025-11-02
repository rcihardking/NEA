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
        float size;

        mesh(const char* filepath, graphicmath::matrix pos, float scale);
        void remove();
        void draw(graphics::shader shader);
    private:
        std::vector<float> verticiesCoords;
        std::vector<float> normalCoords;
        std::vector<float> textureCoords;
        GLuint texture = 0;

        std::vector<int> vertIndex;
        std::vector<int> normIndex;
        std::vector<int> textIndex;
        int polySize = 3;

        GLuint vbo;
        GLuint vao;
        GLuint ebo;

        void parseObj(const char* filepath);
    };
}