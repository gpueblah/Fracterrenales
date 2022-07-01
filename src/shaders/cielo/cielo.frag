#version 330 core
out vec4 FragColor;

in vec4 Position;
in vec2 TexCoords;
in vec3 FragPos;

uniform sampler2D cielo;
uniform float time;
uniform float velocity;
uniform float scale;

uniform float overcast;

uniform vec3 viewPos;

uniform int tipoNiebla;
uniform vec3 FogColor;

uniform float FogMax;
uniform float FogMin;

uniform float FogDensity;

uniform vec3 offset;


float getLinearFogFactor(float c) {
    if (c>=FogMax) return 1;
    if (c<=FogMin) return 0;

    return 1 - (FogMax - c) / (FogMax - FogMin);
}

float getExpFogFactor(float c) {   
    const float e = 2.71828;

    return 1 - pow(e, -FogDensity * c);
}

float getExp2FogFactor(float c) {
    const float e = 2.71828;

    return 1 - pow(e, -FogDensity * pow(c, 2));
}

void main()
{
	float r = texture(cielo, (TexCoords) * scale + time * velocity * vec2(3, 1)).r;

	float alpha = 0.0;
    switch (tipoNiebla) {
        case 1: {
            alpha = getLinearFogFactor(distance(viewPos, FragPos));
            break;
        }
        case 2: {
            alpha = getExpFogFactor(distance(viewPos, FragPos));
            break;
        }
        case 3: {
            alpha = getExp2FogFactor(distance(viewPos, FragPos));
            break;
        }
    }

    vec4 color_cielo = vec4(r > overcast ? 0.0 : 1.0-(overcast-r));
    vec4 niebla = vec4(FogColor, 1.0);

    FragColor = mix(color_cielo, niebla, alpha);
}
