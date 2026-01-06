#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "graphicMath.h"
#include "fileHandler.h"

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
#include <map>

namespace newgraphics {
    class location {
    protected:
        float position[3] = { 0.0f, 0.0f, 0.0f };
        float orientation[3] = { 0.0f, 0.0f, 0.0f };
        float size = 1.0f;

        mat4 translation = iden4();
        mat4 rotation = iden4();
        mat4 scale = iden4();
    public:
        inline vec3 getPosition() {
            vec3 vector = position;
            return vector;
        }
        virtual void move(std::initializer_list<float> pos);
        virtual void move(vec3 pos);
        inline vec3 getOrientation() { 
            vec3 vector = orientation; 
            return vector;
        }
        virtual void rotate(std::initializer_list<float> rot);
        virtual void rotate(vec3 rot);
        inline float getSize() { 
            return size; 
        }
        virtual void resize(float factor);

        mat4 getTransformation();
    };

    struct mesh {
        unsigned int vbo = 0;
        int size = 0;

        unsigned int vao = 0;
    };

    struct texture {
        unsigned int image;
    };

    class camera : public location {
    public:
        void rotate(std::initializer_list<float> rot) override;
        void rotate(vec3 rot) override;
        void move(std::initializer_list<float> pos) override;
        void move(vec3 rot) override;
        void resize(float factor) override;
    };

    class scene {
    public:
        //scene();

        int loadMesh(std::string meshFilepath, GLuint vao); // vao isnt used currently
        int loadImage(std::string imageFilepath);

        mat4 perspective = graphics::createPerspective(toRad(70.0f), 1.0f, 1.0f, 30.0f);
        camera currentCamera;

        std::vector<mesh> meshes;
        std::vector<texture> textures;
    };

    class staticShader {
    public:
        GLuint ID = glCreateProgram();

        staticShader(std::string vertexFilepath, std::string fragmentFilepath);
        ~staticShader();
    }; 
    // change to use function pointers instead

    class staticInstance : public location {
    private:
        std::string identifier;
    protected:
        staticInstance* parent = nullptr;
        std::vector<staticInstance*> children;
    public:
        inline staticInstance(std::string name, scene* scene, staticShader* shader, int mesh, int texture) : identifier{ name }, myScene { scene }, meshIndex{ mesh }, textureIndex{ texture }, myShader{ shader } {};

        staticInstance* search(std::string name);

        void draw();
        void (*drawImplementation)(staticInstance*) = nullptr;

        inline const std::vector<staticInstance*> getChildren() { return static_cast<const std::vector<staticInstance*>>(children); };
        inline staticInstance* getParent() { return parent; };

        void changeParent(staticInstance* newParent);

        void rotate(std::initializer_list<float> rot) override;
        void rotate(vec3 rot) override;
        void move(std::initializer_list<float> pos) override;
        void move(vec3 pos) override;
        void resize(float factor) override;

        // need to move back to private
        scene* myScene = nullptr;
        staticShader* myShader = nullptr;

        int meshIndex;
        int textureIndex;
    };
}

void drawImplementation(newgraphics::staticInstance* self);