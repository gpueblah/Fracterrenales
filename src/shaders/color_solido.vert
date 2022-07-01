#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 pos;


void main()
{
    pos = aPos;
    normal = aNormal;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
    // pos = gl_Position.xyz;
}