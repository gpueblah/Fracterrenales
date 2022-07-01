#version 330 core

void main()
{
    // Ocurre de forma normal si no
    gl_FragDepth = gl_FragCoord.z;
}