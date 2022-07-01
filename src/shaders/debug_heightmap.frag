#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D altura;
uniform float min_value;
uniform float max_value;

float reescale(float value) {
    return (value - min_value) / (max_value - min_value);

}

void main() {       
    float depthValue = texture(altura, TexCoords).r;
    FragColor = vec4(vec3(reescale(depthValue)), 1.0);
}
