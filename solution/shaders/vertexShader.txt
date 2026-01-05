#version 420 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inTex;
layout (location = 2) in vec3 inNor;
  
out vec2 UV;
out vec3 position;
out vec3 normal;


uniform mat4 trans;
uniform mat4 cam;
uniform mat4 proj;

void main()
{
    gl_Position = proj * cam * trans * vec4(inPos, 1.0);
    UV = vec2(inTex.x, 1.0 - inTex.y);

    position = vec3(trans * vec4(inPos, 1.0));
    normal = mat3(transpose(inverse(trans))) * inNor;
}    