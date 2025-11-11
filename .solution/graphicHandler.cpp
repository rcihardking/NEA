#include "graphicHandler.h"

GLuint graphics::shader::compileShader(const char* filePath, GLenum type) { // shaders must be zero terminated
    std::ifstream text(filePath);
    assert(text.is_open());

    std::stringstream buffer;
    buffer << text.rdbuf();
    std::string temp = buffer.str();
    const char* shaderText = temp.c_str();
    text.close();

    GLuint newShader = glCreateShader(type);
    glShaderSource(newShader, 1, &shaderText, NULL);
    glCompileShader(newShader);

    int result; // bool
    glGetShaderiv(newShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        char errLog[512];
        glGetShaderInfoLog(newShader, 512, NULL, errLog);
        std::cout << errLog;
        assert(false);
    }

    shaders.push_back(newShader);
    glAttachShader(ID, newShader);
    return newShader;
}

void graphics::shader::cleanupShaders() {
    for (auto i = shaders.rbegin(); i != shaders.rend(); i++) {
        glDetachShader(ID, *i);
        glDeleteShader(*i);
    }
    shaders.clear();
}

graphics::shader::shader(const char* vertFilepath, const char* fragFilepath) {
    ID = glCreateProgram();
    GLuint vertShader = compileShader(vertFilepath, GL_VERTEX_SHADER);
    GLuint fragShader = compileShader(fragFilepath, GL_FRAGMENT_SHADER);
    glLinkProgram(ID);

    int result; // bool
    glGetProgramiv(ID, GL_LINK_STATUS, &result);
    assert(result);

    cleanupShaders();
}

static inline void objRegex(std::string line, std::regex reg, std::vector<float>* vec, int len) {
    std::smatch matchObj;
    std::regex_match(line, matchObj, reg);

    if (matchObj.size() < 2) {
        assert(false);
    }

    for (int i = 0; i < len; ++i) {
        vec->push_back(std::stof(matchObj[i + 1].str()));
    }
}


void graphics::mesh::parseObj(const char* filepath) {

    std::vector<float> vertexPos;
    std::vector<float> vertexNor;
    std::vector<float> vertexTex;

    std::ifstream obj(filepath);
    assert(obj.is_open());

    static const std::regex v("v (-?[0-1]+.[0-9]*) (-?[0-1]+.[0-9]*) (-?[0-1]+.[0-9]*)");
    static const std::regex vn("vn (-?[0-1]+.[0-9]*) (-?[0-1]+.[0-9]*) (-?[0-1]+.[0-9]*)");
    static const std::regex vt("vt (-?[0-1]+.[0-9]*) (-?[0-1]+.[0-9]*)");
    static const std::regex f("f ([0-9]*/[0-9]*/[0-9]*) ([0-9]*/[0-9]*/[0-9]*) ([0-9]*/[0-9]*/[0-9]*)");
    static const std::regex index("([0-9]*)/([0-9]*)/([0-9]*)");

    for (std::string line; std::getline(obj, line); ) {
        switch (line.c_str()[0] << sizeof(char) * 8 | line.c_str()[1]) {
        case * "v" << sizeof(char) * 8 | *" ": // 30240
            objRegex(line, v, &vertexPos, 3);

            break;

        case * "v" << sizeof(char) * 8 | *"n": // 30318
            objRegex(line, vn, &vertexNor, 3);

            break;

        case * "v" << sizeof(char) * 8 | *"t": // 30324
            objRegex(line, vt, &vertexTex, 2);

            break;

        case * "f" << sizeof(char) * 8 | *" ": // 26144
            std::smatch matchObj;
            std::regex_match(line, matchObj, f);

            if (matchObj.size() < 2) {
                assert(false); // malformed obj file
            }

            std::vector<std::string> str_f;
            for (int i = 0; i < polySize; i++) {
                str_f.push_back(matchObj[i + 1].str());
            }

            for (int i = 0; i < polySize; i++) {
                std::smatch faceIndices;
                std::regex_match(str_f[i], faceIndices, index);

                // need to fix this
                verticies.push_back(vertexPos[(std::stoi(faceIndices[1].str()) - 1) * 3]);
                verticies.push_back(vertexPos[(std::stoi(faceIndices[1].str()) - 1) * 3 + 1]);
                verticies.push_back(vertexPos[(std::stoi(faceIndices[1].str()) - 1) * 3 + 2]);
                
                verticies.push_back(vertexTex[(std::stoi(faceIndices[2].str()) - 1) * 2]);
                verticies.push_back(vertexTex[(std::stoi(faceIndices[2].str()) - 1) * 2 + 1]);
                
                verticies.push_back(vertexNor[(std::stoi(faceIndices[3].str()) - 1) * 3]);
                verticies.push_back(vertexNor[(std::stoi(faceIndices[3].str()) - 1) * 3 + 1]);
                verticies.push_back(vertexNor[(std::stoi(faceIndices[3].str()) - 1) * 3 + 2]);
            }

            break;
        }
    }

}

graphics::mesh::mesh(GLuint shaderID, const char* filepath, GLuint textureID, graphicmath::matrix pos, graphicmath::matrix rotation, float scale) {
    shader = shaderID;
    texture = textureID;
    size = scale;
    orientation = rotation;
    position = pos;

    parseObj(filepath); // can only handle objs so far

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(float), verticies.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void graphics::mesh::remove() {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
}

void graphics::mesh::draw() {
    glUseProgram(shader);
    glBindTexture(GL_TEXTURE_2D, texture);

    const graphicmath::matrix scale = graphicmath::matScale(size);
    const graphicmath::matrix translation = graphicmath::matTranslation(position);
    const graphicmath::matrix proj = graphicmath::matPerspective(toRad(110.0f), 800.0f / 800.0f, 0.5f, 100.0f);

    graphicmath::matrix rotationX = graphicmath::matRotation(graphicmath::vec3({ 1.0f, 0.0f, 0.0f }), orientation.array[0]);
    graphicmath::matrix rotationY = graphicmath::matRotation(graphicmath::vec3({ 0.0f, 1.0f, 0.0f }), orientation.array[1]);
    graphicmath::matrix rotationZ = graphicmath::matRotation(graphicmath::vec3({ 0.0f, 0.0f, 1.0f }), orientation.array[2]);

    graphicmath::matrix rotation = rotationX * (rotationY * rotationZ);

    GLuint loc0 = glGetUniformLocation(shader, "scale");
    GLuint loc1 = glGetUniformLocation(shader, "rotation");
    GLuint loc2 = glGetUniformLocation(shader, "translation");
    GLuint loc3 = glGetUniformLocation(shader, "proj");

    glUniformMatrix4fv(loc0, 1, true, scale.array.data());
    glUniformMatrix4fv(loc1, 1, true, rotation.array.data());
    glUniformMatrix4fv(loc2, 1, true, translation.array.data());
    glUniformMatrix4fv(loc3, 1, true, proj.array.data());

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(verticies.size()));

    glBindTexture(GL_TEXTURE_2D, 0);
}