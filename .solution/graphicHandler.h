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

    class location {
    protected:
        float position[3] = { 0.0f, 0.0f, 0.0f };
        float orientation[3] = { 0.0f, 0.0f, 0.0f };
        float size = 1.0f;

        mat4 translation = createTranslation(0.0f, 0.0f, 0.0f);
        mat4 rotation = createEulerRotation(0.0f, 0.0f, 0.0f);
        mat4 scale = createScale(1.0f);
    
    public:
        inline std::array<float, 3> getPosition() { return *reinterpret_cast<std::array<float, 3>*>(position); }
        void move(std::initializer_list<float> pos);
        inline std::array<float, 3> getOrientation() { return *reinterpret_cast<std::array<float, 3>*>(orientation); }
        void rotate(std::initializer_list<float> rot);
        inline float getSize() { return size; }
        void resize(float factor);
    };

    class mesh : public location {
    public:
        mesh(std::string filepath, GLuint shaderID, GLuint textureID);
        ~mesh();
        void draw();
        
    private:
        GLuint shaderID; // perhaps each scene should be given a shader?
        GLuint textureID;

        GLuint vao;
        GLuint vbo;
        std::vector<float> verticies;
        
        mat4 projection = createPerspective(toRad(70.0f), 1.0f, 1.0f, 30.0f); //need to move this out of each mesh

        int readObj(std::string filepath);
    };

    class instance {
    private:
        std::vector<instance*> children;
        instance* parent;
        int meshIndex;
    };

    class workspace {
    private:
        std::vector<mesh*> meshes;
        instance root;
    };
}