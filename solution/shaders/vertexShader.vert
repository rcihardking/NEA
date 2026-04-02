#version 420 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inTex;
layout (location = 2) in vec3 inNor;
 
//to be sent to the fragment shader
out vec2 UV;
out vec3 position;
out vec3 normal;

//outside inputs
uniform mat4 trans;
uniform mat4 cam;
uniform mat4 proj;

void main()
{
    gl_Position = proj * cam * trans * vec4(inPos, 1.0);    //tells opengl to put the vertex in the correctly transformed placed
    UV = vec2(inTex.x, 1.0 - inTex.y);  //corrects texture as obj has a different format to opengl

    position = vec3(trans * vec4(inPos, 1.0));  //send actual position of vertex to frag shader
    normal = mat3(transpose(inverse(trans))) * inNor;   //send corrected normal to frag shader
}    