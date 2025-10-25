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
            objRegex(line, v, &verticiesCoords, 3);

            break;

        case * "v" << sizeof(char) * 8 | *"n": // 30318
            objRegex(line, vn, &normalCoords, 3);

            break;

        case * "v" << sizeof(char) * 8 | *"t": // 30324
            objRegex(line, vt, &textureCoords, 2);

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

                vertIndex.push_back(std::stoi(faceIndices[1].str()) - 1);
                textIndex.push_back(std::stoi(faceIndices[2].str()) - 1);
                normIndex.push_back(std::stoi(faceIndices[3].str()) - 1);
            }

            break;
        }
    }
}

graphics::mesh::mesh(const char* filepath) {
    parseObj(filepath); // can only handle objs so far

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verticiesCoords.size() * sizeof(float), verticiesCoords.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertIndex.size() * sizeof(int), vertIndex.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
}

void graphics::mesh::remove() {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
}

void graphics::mesh::draw() {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(vertIndex.size()), GL_UNSIGNED_INT, 0);
}