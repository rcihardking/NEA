#pragma once

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

    /*
    class staticMesh : public location {
    public:
        staticMesh(std::string filepath, GLuint shaderID, GLuint textureID, GLuint vao);
        ~staticMesh();
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
    */


    struct mesh {
        GLuint vbo;
        size_t size;

        GLuint vao;
    };

    struct texture { // struct is only here for the sake of extensibility
        GLuint image;
    };

    template<int shaderType>
    struct shader {
        const GLuint ID = glCreateProgram();
        std::vector<GLuint> uniformLocations;

        shader(std::string vertexFilepath, std::string fragmentFilepath);
        ~shader();
    };

    class scene {
    public:
        //scene();
        
        int loadMesh(std::string meshFilepath, GLuint vao);
        int loadImage(std::string imageFilepath);

        mat4 perspective = createPerspective(toRad(70.0f), 1.0f, 1.0f, 30.0f);

        std::vector<mesh> meshes;
        std::vector<texture> textures;

        template <int shaderType>
        shader<shaderType> sceneShader();
    };

    class staticInstance : public location {
    private:
        scene* myScene;

        staticInstance* parent;
        std::vector<staticInstance*> children;

        GLuint shader;
        int meshIndex;
        int textureIndex;
    public:
        inline staticInstance(scene* scene, int mesh, int texture, GLuint shaderID) : myScene{ scene }, meshIndex { mesh }, textureIndex{ texture }, shader{ shaderID } {};

        void draw();

        const std::vector<staticInstance*> getChildren();
        staticInstance* getParent();

        void changeParent(staticInstance* newParent);
        void removeChild(staticInstance* child);
        void addChild(staticInstance* child);
    };
}