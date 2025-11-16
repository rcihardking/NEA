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

/*
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
*/

namespace graphics {
    struct shader {
        const GLuint ID = glCreateProgram();

        shader(std::string vertexFilepath, std::string fragmentFilepath);
        ~shader();
    };

    class mesh {
    public:
        mesh(GLuint ID, std::string filepath, GLuint texture, std::initializer_list<float> pos, std::initializer_list<float> rot, float factor);
        ~mesh();

        void draw();

        
        //std::array<float, 3> position();
        //void move(std::initializer_list<float> pos);
        //std::array<float, 3> orientation();
        //void rotate(std::initializer_list<float> rot);
        //float size();
        //void scale(float factor);
        
    private:
        GLuint shaderID;
        GLuint textureID;

        GLuint vao;
        std::vector<float> verticies;
        GLuint vbo;

        float position[3]; // change these variables to std::array
        float orientation[3];
        float size;

        mat4 translation;
        mat4 rotation;
        mat4 scale;
        
        mat4 projection = createPerspective(70.0f, 1.0f, 0.0f, 50.0f); //temporary, need to move this out of each mesh

        int readObj(std::string filepath);
    };
}