#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "graphicMath.h"
#include "fileHandler.h"
#include "fileHandler_NEW.h"

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

namespace graphics {
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

    struct texture {
        unsigned int img = 0;
    };

    struct mesh {
        unsigned int vao = 0;
        unsigned int vbo = 0;
        //unsigned int ebo = 0;
        int offset = 0;
        int size = 0;
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

        std::vector<graphics::mesh> meshes;
        std::vector<graphics::texture> textures;
    };

    class shader {
    public:
        GLuint ID = glCreateProgram();

        shader(std::string vertexFilepath, std::string fragmentFilepath);
        ~shader();
    }; 
    // change to use function pointers instead

    class instance : public location {
    private:
        std::string identifier;
    protected:
        instance* parent = nullptr;
        std::vector<instance*> children;
    public:
        inline instance(std::string name, scene* scene, shader* shader, int mesh, int texture) : identifier{ name }, myScene { scene }, meshIndex{ mesh }, textureIndex{ texture }, myShader{ shader } {};

        instance* search(std::string name);

        void draw();
        void (*drawImplementation)(instance*) = nullptr;

        inline const std::vector<instance*> getChildren() { return static_cast<const std::vector<instance*>>(children); };
        inline instance* getParent() { return parent; };

        void changeParent(instance* newParent);

        void rotate(std::initializer_list<float> rot) override;
        void rotate(vec3 rot) override;
        void move(std::initializer_list<float> pos) override;
        void move(vec3 pos) override;
        void resize(float factor) override;

        // need to move back to private
        scene* myScene = nullptr;
        shader* myShader = nullptr;

        int meshIndex;
        int textureIndex;
    };
}

void drawImplementation(graphics::instance* self);