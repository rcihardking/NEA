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
    struct shader {
        const GLuint ID = glCreateProgram();

        shader(std::string vertexFilepath, std::string fragmentFilepath);
        ~shader();
    };

    /*
    struct texture {
        GLuint ID;


    };

    struct mesh {
        GLuint vao;
        GLuint vbo;
        int size;

        mesh(std::string meshFilepath);
        ~mesh();
    };
    */

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
        virtual void move(std::initializer_list<float> pos);
        inline std::array<float, 3> getOrientation() { return *reinterpret_cast<std::array<float, 3>*>(orientation); }
        virtual void rotate(std::initializer_list<float> rot);
        inline float getSize() { return size; }
        virtual void resize(float factor);
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
        mat4 transformation;

        int readObj(std::string filepath);
    };
    
    /*
    class instance : public location {
    private:
        instance* parent;
        std::vector<instance*> children;
    public:
        mesh* meshPtr;
        image* texturePtr;
        shader* shaderPtr;

        bool instanced = false;

        void changeParent();
        inline instance* getParent() { return parent; }
        void addChild();
        void removeChild();
        inline std::vector<instance*> getChildren() { return children; }

        void draw();
        
        instance();
        ~instance();
    };
    */
    
}