#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "graphicMath.h"
#include "fileHandler_NEW.h"

#include <glad/glad.h>
#include <libpng/png.h>
#include <glfw/glfw3.h>

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
    class scene {
    public:
        scene(GLFWwindow* renderingWindow = nullptr, instance* rootInstance = nullptr) : window{ renderingWindow }, root{ rootInstance } {};
        inline void changeRoot(instance* newRoot) {
            root = newRoot;
        };
        inline void changeWindow(GLFWwindow* newWindow) {
            window = newWindow;
        };
        inline void drawScene() {
            if (root != nullptr || window != nullptr) {
                root->draw();
                return;
            }
            std::cout << "trying to draw with incomplete scene\n";
        };

        camera currentCamera;
    private:
        instance* root;
        GLFWwindow* window;
    };

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

    class camera : public location {
    public:
        void rotate(std::initializer_list<float> rot) override;
        void rotate(vec3 rot) override;
        void move(std::initializer_list<float> pos) override;
        void move(vec3 rot) override;
        void resize(float factor) override;

        mat4 getPerspective();
        void changePerspective(float fov, float aspect, float near, float far);
    private:
        mat4 perspective;
    };

    class instance : public location {
    private:
        std::string identifier;
        scene* myScene;
        mesh myMesh;
        texture myTexture;
        shader myShader;
        void (*drawImplementation)(instance*, scene*, mesh, texture, shader) = nullptr;

    protected:
        instance* parent = nullptr;
        std::vector<instance*> children;

    public:
        inline instance(std::string name, scene* scene, mesh model, texture image, shader shad) : identifier{ name }, myScene{ scene }, myMesh{ model }, myTexture{ image }, myShader{ shad } {};

        instance* search(std::string name);

        void draw();

        inline const std::vector<instance*> getChildren() { return static_cast<const std::vector<instance*>>(children); };
        inline instance* getParent() { return parent; };

        void changeParent(instance* newParent);

        void rotate(std::initializer_list<float> rot) override;
        void rotate(vec3 rot) override;
        void move(std::initializer_list<float> pos) override;
        void move(vec3 pos) override;
        void resize(float factor) override;
    };
}

void drawImplementation(graphics::instance* self, graphics::scene* myScene, mesh myMesh, texture myTexture, shader myShader);