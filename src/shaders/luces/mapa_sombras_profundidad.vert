#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

uniform float minHeight;
uniform float maxHeight;

void main()
{
    // float s = smoothstep(minHeight, maxHeight, aPos.y);
    // gl_Position = lightSpaceMatrix * model * vec4(aPos.x, s <= 0.25 ? mix(minHeight, maxHeight, 0.25) : aPos.y, aPos.z, 1.0);
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}