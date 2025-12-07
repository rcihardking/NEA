#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "graphicMath.h"

#include <glad/glad.h>
#include <libpng/png.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>
#include <vector>
#include <array>
#include <regex>


namespace graphics {
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

    struct mesh {
        GLuint vbo = 0;
        GLsizei size = 0;

        GLuint vao = 0;
    };

    struct texture { // struct is only here for the sake of extensibility
        GLuint image;
    };

    class scene {
    public:
        //scene();
        
        int loadMesh(std::string meshFilepath);
        int loadImage(std::string imageFilepath);

        mat4 perspective = createPerspective(toRad(70.0f), 1.0f, 1.0f, 30.0f);

        std::vector<mesh> meshes;
        std::vector<texture> textures;
    };

    struct staticShader {
        GLuint ID = glCreateProgram();
        std::vector<GLuint> uniformLocations;

        staticShader(std::string vertexFilepath, std::string fragmentFilepath);
        ~staticShader();
    };


    class staticInstance : public location {
    private:
        scene* myScene;
        staticShader* myShader;

        staticInstance* parent = nullptr;
        std::vector<staticInstance*> children;

        int meshIndex;
        int textureIndex;
    public:
        inline staticInstance(scene* scene, staticShader* shader, int mesh, int texture) : myScene{ scene }, meshIndex{ mesh }, textureIndex{ texture }, myShader{ shader } {};

        void draw();

        inline const std::vector<staticInstance*> getChildren() { return static_cast<const std::vector<staticInstance*>>(children); };
        inline staticInstance* getParent() { return parent; };

        void removeChild(staticInstance* child);
        void addChild(staticInstance* child);
        void changeParent(staticInstance* newParent);
    };
}